#include "header.h"


const int long_size = sizeof(long);
unsigned long ip;
int etat;
char info [20];
unsigned icounter = 0,instr;
void* start = NULL;
int i, fd;
struct stat state;
char *strtab;
int nb_symbols;


void procmsg(const char* format, ...)
{
    va_list ap;
    fprintf(stdout, "[%d] ", getpid());
    va_start(ap, format);
    vfprintf(stdout, format, ap);
    va_end(ap);
}
void run_code (char* prog_name) {

    if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
        printf("Erreur Ptrace\n");
    
    }
	printf("---------------------------------------------------------------------------------\n");       	

char * args [] = { prog_name  , NULL } ;   	 
	execv ( args[0] , args ) ;
}



void affichage(char* programme) {

   struct user_regs_struct regs;
   

    printf("RIP: 0x%016llx RSP: 0x%016llx RBP: 0x%016llx \n",
                regs.rip, regs.rsp, regs.rbp);
}	

void run_debugger(pid_t child_pid, char* prog_name)
{

    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
    
    printf("---------------------------------------------------------------------------------\n");       	
    printf("\n		Debugger started    		\n");
    printf("\n---Appuyer sur h ou taper help pour voir les differentes options---------------\n");


    /* Attend que l'enfant s'arrête sur sa première instruction */
    wait(&etat);
    
    while(1){
    
     
    printf("(MiniGDB) : ");
    int unused __attribute__((unused));
    unused =scanf("%s",info);
    
    if(strcmp(info,"h")==0 || strcmp(info,"help")==0){
    	printf("\nBienvenue sur mon MiniGDB, voici les commandes qu'on peut utiliser :\n");
	  
	    printf("\na - Affiché les information a propos de PID\n");
            printf("s - singlestep\n");
            printf("add - adresse de depart, fin et nb d'instruction\n");
            printf("r - registers\n");
            printf("run - executer le code \n");
            printf("c - continuer\n");
            printf("bib - information lier a readelf\n");            
            printf("b - breakpoint \n");
            printf("q - quitter\n\n");   
  
    }else if(strcmp(info,"r")==0){
            printf("RAX: 0x%016llx RBX: 0x%016llx RCX: 0x%016llx RDX: 0x%016llx\n",
                        regs.rax, regs.rbx, regs.rcx, regs.rdx);
            printf("R15: 0x%016llx R14: 0x%016llx R13: 0x%016llx R12: 0x%016llx\n",
                        regs.r15, regs.r14, regs.r13, regs.r12);
            printf("R11: 0x%016llx R10: 0x%016llx R9:  0x%016llx R8:  0x%016llx\n",
                        regs.r11, regs.r10, regs.r9, regs.r8);
            printf("RSP: 0x%016llx RBP: 0x%016llx RSI: 0x%016llx RDI: 0x%016llx\n",
                        regs.rsp, regs.rbp, regs.rsi, regs.rdi);
            printf("RIP: 0x%016llx CS: 0x%04llx EFLAGS: 0x%08llx\n",
                        regs.rip, regs.cs, regs.eflags);
            printf("SS:  0x%04llx DS: 0x%04llx ES: 0x%04llx FS: 0x%04llx GS: 0x%04llx\n",
                        regs.ss, regs.ds, regs.es, regs.fs, regs.gs);
    
    
    }else if(strcmp(info,"s")==0){
    	   
           ptrace(PTRACE_SINGLESTEP, child_pid, 0, 0);
           wait(&etat);
	   affichage(prog_name);
    
    }else if(strcmp(info,"c")==0){
    
    
	 ptrace(PTRACE_CONT, child_pid, 0, 0);
                if (WIFEXITED(etat)) {
                                  
                    affichage(prog_name);
                } else {
                 perror( "Continu " );
                }
        }else if(strcmp(info,"add")==0){
       
	while (WIFSTOPPED(etat)) {

		icounter++;   	
		
           	ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
	
		instr = ptrace(PTRACE_PEEKTEXT, child_pid,regs.rip, 0);  
		if (icounter == 1){
		printf(" L'adresse de départ RIP = 0x%08llx RSP: 0x%08llx instruction: 0x%08x \n",
	              regs.rip, regs.rsp, instr);
	         }     
	         
		ptrace(PTRACE_SINGLESTEP, child_pid, 0, 0);
      
      /* Attend que l'enfant s'arrête sur sa prochaine instruction */
	        wait(&etat);


	}
	
	printf(" L'adresse de la fin RIP = 0x%08llx RSP: 0x%08llx instruction: 0x%08x \n",
	              regs.rip, regs.rsp, instr);
              
	printf("\n	L'enfant a executé %u instructions\n\n", icounter);
   
     }else if(strcmp(info,"a")==0){

	printf("\n--------------------------");
	printf("\n PID   ||   PPID  || PPPID ");
	printf("\n--------------------------");
	printf("\n %d ||  %d  ||  %d ",child_pid,getpid(),getppid());
	printf("\n--------------------------\n");
     
     }else if(strcmp(info,"run")==0){
	char * args [] = { prog_name  , NULL } ;   	 
	execv ( args[0] , args ) ;
     
     }else if(strcmp(info,"b")==0){
     	    printf("(MiniGDB) : break at ");
     	    unsigned addr;
            unused =scanf("%d",&addr);
            
            unsigned data = ptrace(PTRACE_PEEKTEXT, child_pid, addr, 0);
            if(addr == data ){            printf("After trap, data at 0x%08x\n", data);}
            else {
                        printf("L'adresse n'existe pas\n");
            }

     }else if(strcmp(info,"q")==0){
     
     break;
     }else if(strcmp(info,"bib")==0){
     
	// ouverture du fichier (pour être mappé)
	fd = open(prog_name, O_RDONLY, 660);
	if(fd < 0)
		perror("open");

	// récupération de la taille du fichier
	fstat(fd, &state);

	//projection du fichier (MAP_SHARED importe peu ici)
	start = mmap(0, state.st_size, PROT_READ , MAP_FILE | MAP_SHARED, fd, 0);
	if(start == MAP_FAILED)
	{
		perror("mmap");
		abort();
	}


		Elf64_Ehdr* hdr = (Elf64_Ehdr *) start;
		Elf64_Sym* symtab;


		//printf("Check four first bytes: %x '%c' '%c' '%c'\n", (char)start,*((char*)start+1), ((char)start+2), ((char)start+3));


		Elf64_Shdr* sections = (Elf64_Shdr *)((char *)start + hdr->e_shoff);		


		for (i = 0; i < hdr->e_shnum; i++)
		{
			if (sections[i].sh_type == SHT_SYMTAB) {
				symtab = (Elf64_Sym *)((char *)start + sections[i].sh_offset);
				nb_symbols = sections[i].sh_size / sections[i].sh_entsize;

				strtab = (char*)((char*)start + sections[sections[i].sh_link].sh_offset);
			}
		}	

		for (i = 0; i < nb_symbols; ++i) {
			printf("%d: %s\n", i, strtab + symtab[i].st_name);
		}

	}else{
     
     		printf("La commande n'existe pas !!  \n");
     	}
	
    }
  /*
    

*/
}





int main(int argc, char* argv[]) {

    if (argc < 2) {
	printf("nombre de parametre insuffisant...\n");
	return -1;
    }
	
   char* prog_child = argv[1];
	
   pid_t pid = fork();
   

   
   if (pid == 0) {
	//child process
		run_code(prog_child);
	
    }
    else if (pid > 0)  {
    //parent process
		
		printf("  Démarrage de debuggeur %d pour le programme %s \n",pid,prog_child);
		run_debugger(pid,prog_child);
 
        
    }else {    
		perror( "fork" );
		return -1;
    }
    

	
	

return 0;
    
}
