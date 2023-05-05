#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#define _GNU_SOURCE

struct process {
	char *p_name;
	char *p_pid;
	struct process *bro;
	struct process *son;
};
typedef struct process process;

// tools

int listdir(const char*, const char*, process**, int, int);
int check_digital(const char*);
int build_tree(const char*, process*, int dep);
// struct process function 
process* create_p(const char*, const char*);
int insert(process*, process*, const char*);
int print_tree(process*, int, int*, int);


int main(int argc, char *argv[]) {
  for (int i = 0; i < argc; i++) {
    assert(argv[i]);
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  assert(!argv[argc]);

  int buf[1024] = {0};
	int state_n = 0, state_p = 0;
	for (int i = 1; i < argc; ++ i) {
		if (strcmp(argv[i], "-n") == 0) state_n = 1;
		else if (strcmp(argv[i], "-p") == 0) state_p = 1;
		else {
			puts(argv[i]);
			puts("errof command");
			exit(0);
		}
	}
	process** head = (process**)malloc(sizeof(process*));
	listdir("/proc", " ", head, 0, state_p);
	printf("\n");
	print_tree((*head), 0, buf, state_n);
	printf("\n");
	return 0;
}

int listdir(const char* path, const char* father, process** head, int dep, int flag) {
	struct dirent *entry;
	DIR *dir;
	dir = opendir(path);
	if (dir == NULL) {
		perror("fail opendir");
		return -1;
	}
	if (dep == 0) *head = create_p("", "0"); // init process
	while ((entry = readdir(dir))) 
		if (DT_DIR == entry -> d_type)
			if (check_digital(entry->d_name) != -1) {
				if (dep == 1 && !strcmp(entry->d_name, father))
					continue;
				build_tree(entry->d_name, *head, dep);
				if (0 == dep && flag) {
					char buf[1024];
					strcpy(buf, "/proc/");
					strcat(buf, entry->d_name);
					strcat(buf, "/task");
					listdir(buf, entry->d_name, head, 1, flag);
				}
			}
	closedir(dir);
	return 0;
}

int check_digital(const char* s) {
	int flag = 0;
	for (; *s; ++ s)
		if (*s < '0' || *s > '9')
			return -1;
	return 0;
}

int build_tree(const char* path, process* head, int dep) {
	const int size = 1024;
	FILE* fp;
	char ch[size];
	strcpy(ch, "/proc/");
	strcat(ch, path);
	strcat(ch, "/status");
	fp = fopen(ch, "r");
	char pid[size], ppid[size], name[size];
	while((fscanf(fp, "%s", ch)) != EOF) {
		if (strcmp(ch, "Pid:") == 0) {
			fscanf(fp, "%s", pid);
		}
		if (strcmp(ch, "PPid:") == 0) {
			fscanf(fp, "%s", ppid);
		}
		if (strcmp(ch, "Name:") == 0) {
			fscanf(fp, "%s", name);
		}
	}
	ch[0] = 0;
	if (dep) strcpy(ch, "{");
	strcat(ch, name);
	if (dep) strcat(ch, "}");
	insert(head, create_p(ch, pid), ppid);
	fclose(fp);
	return 0;
}

void check_memory(void* p) {
	if (p == NULL) {
		perror("malloc error");
		exit(0);
	}
}

process* create_p(const char* p_name, const char* p_pid) {
	process* p = (process*)malloc(sizeof(process));
	check_memory(p);
	p->p_name = (char*)malloc(sizeof(char) * (strlen(p_name) + 1));
	p->p_pid  = (char*)malloc(sizeof(char) * (strlen(p_pid)  + 1));
	check_memory(p->p_name);
	check_memory(p->p_pid);
	strcpy(p->p_name, p_name);
	strcpy(p->p_pid,  p_pid);
	p->bro = NULL;
	p->son = NULL;
	return p;
}

// tools
int cmp(const char* a, const char *b) {
	int tempa = atoi(a), tempb = atoi(b);
	if (a == b) return 0;
	if (a > b) return 1;
	return -1;
}
void insert_node(process* head, process* node) { 
	/*help insert node function*/
	process* cur = head->bro;
	process* prv = head;
	while (cur) {
		int state = strcmp(head->p_name, node->p_name);
		if ( 1 == state || 
			(0 == state && -1 == cmp(head->p_pid, node->p_name))
		   ) {
			prv->bro = node;
			node->bro = cur;
			return;
		}
		prv = cur;
		cur = cur->bro;
	}
	prv->bro = node;
	return;
}

int insert(process* head, process* node, const char* fp_pid) {
	if (node == NULL) return -1;
	if (head == NULL) return -1;
	while (head) {
		if (0 == strcmp(head->p_pid, fp_pid)) {
			if (head->son)
				insert_node(head->son, node);
			else 
				head->son = node;
			return 0;
		} else {
			if (0 == insert(head->son, node, fp_pid))
				return 0;
		}
		head = head->bro;
	}
	return -1;
}

int print_tree(process* head, int len, int* buf, int flag) {
	if (head == NULL) return -1;
	
	while (head) {
		printf("%s", head->p_name);
		if (flag) printf("(%s)", head->p_pid);
		if (head->son) {
			if(!head->son->bro) printf("\u2500\u2500\u2500");
			else printf("\u2500\u252c\u2500");
			int size = strlen(head->p_name) + len + 3 + (flag * (strlen(head->p_pid) + 2));
			if (head->son->bro) buf[size - 2] = 1;
			print_tree(head->son, size, buf, flag);
		}
		head = head->bro;
		if (head) {
			printf("\n");
			if (!head->bro) buf[len - 2] = 0;
			for (int i = 0; i < len - 2; ++ i) 
				if (buf[i]) printf("\u2502");
				else printf(" ");
			if (!head->bro) printf("\u2514\u2500");
			else printf("\u251c\u2500");
		}
	}


  return 0;
}
