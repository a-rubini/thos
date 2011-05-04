
extern int thos_main(void);
extern int thos_setup(void);

extern void putc(int c);
extern void puts(char *s);

extern volatile unsigned long jiffies;

struct thos_task {
	char *name;
	void *(*job)(void *);
	int (*init)(void *);
	void *arg;
	unsigned long period;
	unsigned long release;
};

#define __task __attribute__((section(".task"),__used__))

extern struct thos_task __task_begin[], __task_end[];
