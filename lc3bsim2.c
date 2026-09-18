/*
    Name 1: Kritika Gautam
    Name 2: Samana Poudal
    UTEID 1: KG34634
	  UTEID 2: SP52752
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
*/

#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

  int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
  int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {                                                    
  printf("----------------LC-3b ISIM Help-----------------------\n");
  printf("go               -  run program to completion         \n");
  printf("run n            -  execute program for n instructions\n");
  printf("mdump low high   -  dump memory from low to high      \n");
  printf("rdump            -  dump the register & bus values    \n");
  printf("?                -  display this help menu            \n");
  printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  process_instruction();
  CURRENT_LATCHES = NEXT_LATCHES;
  INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
  int i;

  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating for %d cycles...\n\n", num_cycles);
  for (i = 0; i < num_cycles; i++) {
    if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
    }
    cycle();
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating...\n\n");
  while (CURRENT_LATCHES.PC != 0x0000)
    cycle();
  RUN_BIT = FALSE;
  printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
  int address; /* this is a byte address */

  printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  printf("-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  printf("\n");

  /* dump the memory contents into the dumpsim file */
  fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  fprintf(dumpsim_file, "-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
  int k; 

  printf("\nCurrent register/bus values :\n");
  printf("-------------------------------------\n");
  printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
  printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P); 

  printf("Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
  fprintf(dumpsim_file, "-------------------------------------\n");
  fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
  fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  fprintf(dumpsim_file, "Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
  char buffer[20];
  int start, stop, cycles;

  printf("LC-3b-SIM> ");

  scanf("%s", buffer);
  printf("\n");

  switch(buffer[0]) {
  case 'G':
  case 'g':
    go();
    break;

  case 'M':
  case 'm':
    scanf("%i %i", &start, &stop);
    mdump(dumpsim_file, start, stop);
    break;

  case '?':
    help();
    break;
  case 'Q':
  case 'q':
    printf("Bye.\n");
    exit(0);

  case 'R':
  case 'r':
    if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
    else {
	    scanf("%d", &cycles);
	    run(cycles);
    }
    break;

  default:
    printf("Invalid Command\n");
    break;
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
  int i;

  for (i=0; i < WORDS_IN_MEM; i++) {
    MEMORY[i][0] = 0;
    MEMORY[i][1] = 0;
  }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {                   
  FILE * prog;
  int ii, word, program_base;

  /* Open program file. */
  prog = fopen(program_filename, "r");
  if (prog == NULL) {
    printf("Error: Can't open program file %s\n", program_filename);
    exit(-1);
  }

  /* Read in the program. */
  if (fscanf(prog, "%x\n", &word) != EOF)
    program_base = word >> 1;
  else {
    printf("Error: Program file is empty\n");
    exit(-1);
  }

  ii = 0;
  while (fscanf(prog, "%x\n", &word) != EOF) {
    /* Make sure it fits. */
    if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
             program_filename, ii);
	    exit(-1);
    }

    /* Write the word to memory array. */
    MEMORY[program_base + ii][0] = word & 0x00FF;
    MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
    ii++;
  }

  if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

  printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */ 
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *files[], int num_prog_files) { 
  int i;

  init_memory();
  for ( i = 0; i < num_prog_files; i++ ) {
    load_program(files[i]);
  }
  CURRENT_LATCHES.Z = 1;  
  NEXT_LATCHES = CURRENT_LATCHES;
    
  RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
  FILE * dumpsim_file;

  /* Error Checking */
  if (argc < 2) {
    printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
           argv[0]);
    exit(1);
  }

  printf("LC-3b Simulator\n\n");

  initialize(&argv[1], argc - 1);

  if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
    printf("Error: Can't open dumpsim file\n");
    exit(-1);
  }

  while (1)
    get_command(dumpsim_file);
    
}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

/***************************************************************/

void decode_instr(int instr, int opcode, int *real_arg1, int *real_arg2, int *real_arg3, int *real_flag);

int SEXT5(int val);
int SEXT6(int val);
int SEXT9(int val);
int SEXT11(int val);
void update_cc(int val);

void execute_add(int instr, int dr, int sr1, int op2, int flag);
void execute_and(int instr, int dr, int sr1, int op2, int flag); 
void execute_branch(int arg1, int pc_offset);
void execute_jump(int base_register);
void execute_jsr(int arg1, int flag);
void execute_ldb(int dr, int baser, int offset); 
void execute_ldw(int dr, int baser, int offset);
void execute_lea(int dr, int pcoffset);
void execute_xor(int dr, int sr1, int op2, int flag);
void execute_shift(int dr, int sr, int amount, int flag); 
void execute_stb(int sr, int baser, int offset); 
void execute_stw(int sr, int baser, int offset); 
void execute_trap(int trap_vector); 

void process_instruction(){ // runs once every cycle, runs per new instruction
  /*  function: process_instruction
   *  
   *    Process one instruction at a time  
   *       -Fetch one instruction
   *       -Decode 
   *       -Execute
   *       -Update NEXT_LATCHES
   */   

  //fetch
  int ii = CURRENT_LATCHES.PC >> 1; //bc memory is word addressable BUT PC IS BYTE ADDRESSABLE, we need to shift right by 1 to get the word address
  // pc incremented in fetch stage
  NEXT_LATCHES.PC = LowBits(CURRENT_LATCHES.PC + 2); 

  int instr = 0; 
  // little endian!! 0 = lsb 1 = msb
  instr = ((MEMORY[ii][1] & 0xFF) << 8)| (MEMORY[ii][0] & 0xFF); // puts together a 16 bit instruction = hex 4 digit

  //decode
  int arg1;
  int arg2;
  int arg3;
  int flag; // used for two types of and etc
  int opcode = (instr >> 12) & 0xF;
  decode_instr(instr, opcode, &arg1, &arg2, &arg3, &flag); // decode the instruction into assembly instructions and arguments

  // execute

  switch(opcode){
    case 1: // add
      execute_add(instr, arg1, arg2, arg3, flag);
      break;
    case 5: // and
      execute_and(instr, arg1, arg2, arg3, flag);
      break;
    case 0: // br
      execute_branch(arg1, arg2);
      break;
    case 12: // jmp or ret
      execute_jump(arg1);
      break;
    case 4: // jsrr or jsr
      execute_jsr(arg1, flag);
      break;
    case 2: // ldb BYTE
      execute_ldb(arg1, arg2, arg3);
      break;
    case 6: // ldw WORD
      execute_ldw(arg1, arg2, arg3);
      break;
    case 14: // lea
      execute_lea(arg1, arg2);
      break;
    case 9: // not or xor
      execute_xor(arg1, arg2, arg3, flag);
      break;
    case 8: // rti not implemented
      break;
    case 13: // shift
      execute_shift(instr, arg1, arg2, arg3);
      break;
    case 3: // stb BYTE
      execute_stb(arg1, arg2, arg3);
      break;
    case 7: // stw WORD
      execute_stw(arg1, arg2, arg3);
      break;
    case 15: // trap
      execute_trap(arg1);
      break; 
  } 
  //updating next latches is done in the execute functions and cycle automatically to the next latches

}


void decode_instr(int instr, int opcode, int *real_arg1, int *real_arg2, int *real_arg3, int *real_flag){
  ///decode: from machine code to assembly instructions 
  int arg1 = 0;
  int arg2 = 0;
  int arg3 = 0;
  int flag = -1;
 
  switch(opcode){
    case 1: // add
      if ((instr & 0x0020) == 0){ // if bit 5 is 0, then we are using register mode
        // dr sr1 sr2
        flag = 0; 
        arg1 = (instr & 0x00E00) >> 9; 
        arg2 = (instr & 0x001C0) >> 6; 
        arg3 = (instr & 0x00007);
      } else { // immediate mode
        flag = 1; 
        arg1 = (instr & 0x00E00) >> 9; 
        arg2 = (instr & 0x001C0) >> 6; 
        arg3 = (instr & 0x0001F); // imm5
      }
      break;
    case 5: // and
      if((instr & 0x0020) == 0){ // if bit 5 is 0, then we are using register mode
        // dr sr1 sr2
        flag = 0; 
        arg1 = (instr & 0x00E00) >> 9; 
        arg2 = (instr & 0x001C0) >> 6; 
        arg3 = (instr & 0x00007);
      } else { // immediate mode
        flag = 1; 
        arg1 = (instr & 0x00E00) >> 9; 
        arg2 = (instr & 0x001C0) >> 6; 
        arg3 = (instr & 0x0001F); // imm5
      }
      break;
    case 0: // br
      arg1 = (instr & 0x0E00) >> 9; // n,z,p
      arg2 = (instr & 0x01FF); // pc offset
      break;
    case 12: // jmp or ret
      if((instr & 0x01C0) == 0){ // ret
        arg1 = 7; 
      } else { // jmp
        arg1 = (instr & 0x01C0) >> 6; 
      }
      break;
    case 4:
      if((instr & 0x0800) == 0){ 
        flag = 0; 
        arg1 = (instr & 0x01C0) >> 6; //jsrr
      } else { // jsr
        flag = 1; 
        arg1 = (instr & 0x01FF);  
      }
      break;
    case 2: // ldb BYTE
      arg1 = (instr & 0x00E00) >> 9; 
      arg2 = (instr & 0x001C0) >> 6; 
      arg3 = (instr & 0x0003F); 
      break;
    case 6: // ldw WORD
      arg1 = (instr & 0x00E00) >> 9; 
      arg2 = (instr & 0x001C0) >> 6; 
      arg3 = (instr & 0x0001FF); 
      break;
    case 14: // lea
      arg1 = (instr & 0x00E00) >> 9;
      arg2 = (instr & 0x01FF); 
      break;
    case 9: // not or xor
      if((instr & 0x0020 ) == 0){ // xor with two source registers
        flag = 0; 
        arg1 = (instr & 0x00E00) >> 9;
        arg2 = (instr & 0x001C0) >> 6;
        arg3 = (instr & 0x00007);
      }else{
        flag = 1; 
        arg1 = (instr & 0x00E00) >> 9;
        arg2 = (instr & 0x001C0) >> 6;
        arg3 = (instr & 0x0001F); // imm5. can be 11111 if not instr
      }
      break;
    // rti not implemented bc programs wont use :)
    case 13: // shift
      if((instr & 0x0030) == 0){ // lshf
        flag = 0;
        arg1 = (instr & 0x00E00) >> 9; 
        arg2 = (instr & 0x001C0) >> 6; 
        arg3 = (instr & 0x000F); 
      } else if((instr & 0x0030) == 16){ // rshfl
        flag = 1;
        arg1 = (instr & 0x00E00) >> 9; 
        arg2 = (instr & 0x001C0) >> 6; 
        arg3 = (instr & 0x000F); 
      } else { // rshfa
        flag = 3; 
        arg1 = (instr & 0x00E00) >> 9; 
        arg2 = (instr & 0x001C0) >> 6; 
        arg3 = (instr & 0x000F); 
      }
      break;
    case 3: // stb BYTE
      arg1 = (instr & 0x00E00) >> 9;
      arg2 = (instr & 0x001C0) >> 6;
      arg3 = (instr & 0x0003F); 
      break;
    case 7: // stw WORD
      arg1 = (instr & 0x00E00) >> 9;
      arg2 = (instr & 0x001C0) >> 6;
      arg3 = (instr & 0x0001FF);
      break;
    case 15: // trap
      arg1 = (instr & 0x00FF); // 8 bit trap vector 
      break;

  }
  *real_arg1 = arg1;
  *real_arg2 = arg2;
  *real_arg3 = arg3;
  *real_flag = flag;
  return; 
}

int SEXT5(int val){
  return (val & 0x10) ? (val | 0xFFE0) : (val & 0x001F);
}


int SEXT6(int val){
  return (val & 0x20) ? (val | 0xFFC0) : (val & 0x003F);
}

int SEX8(int val){
  return (val & 0x80) ? (val | 0xFF00) : (val & 0x00FF);
}


int SEXT9(int val){
  return (val & 0x100) ? (val | 0xFE00) : (val & 0x01FF);
}
int SEXT11(int val){
  return (val & 0x400) ? (val | 0xF800) : (val & 0x07FF);
}


void update_cc(int val){
  val = Low16bits(val);
  if(val < 0){
    NEXT_LATCHES.N = 1;
    NEXT_LATCHES.Z = 0;
    NEXT_LATCHES.P = 0;
  } else if(val == 0){
    NEXT_LATCHES.N = 0;
    NEXT_LATCHES.Z = 1;
    NEXT_LATCHES.P = 0;
  } else {
    NEXT_LATCHES.N = 0;
    NEXT_LATCHES.Z = 0;
    NEXT_LATCHES.P = 1;
  }
}


// args are register number like the actual 0, 1 etc
// dont need pointers since were not changing the args just the actual memory which is global
void execute_add(int instr, int dr, int sr1, int op2, int flag){
  // add dr, sr1, op2
  int op1_val = CURRENT_LATCHES.REGS[sr1];
  int op2_val = flag ? SEXT5(op2) : CURRENT_LATCHES.REGS[op2]; // flag = 0 means reg mode
  int res = Low16bits(op1_val + op2_val);
  NEXT_LATCHES.REGS[dr] = res;
  update_cc(res);
}

void execute_and(int instr, int dr, int sr1, int op2, int flag){
  // and dr, sr1, op2
  int op1_val = CURRENT_LATCHES.REGS[sr1];
  int op2_val = flag ? SEXT5(op2) : CURRENT_LATCHES.REGS[op2];
  int res = Low16bits(op1_val & op2_val);
  NEXT_LATCHES.REGS[dr] = res;
  update_cc(res);
}

void execute_branch(int arg1, int pc_offset){
  // arg1 = nzp bits, arg2 = pc offset
  int n = (arg1 >> 2) & 0x01; 
  int z = (arg1 >> 1) & 0x01;
  int p = arg1 & 0x01;
  int ben = (n & CURRENT_LATCHES.N) | (z & CURRENT_LATCHES.Z) | (p & CURRENT_LATCHES.P);
  if(ben){
    // pc <- pc + lshf(off9,1) lshf bc we need to multiply by 2 since label for offset is based on address, not pc value
    NEXT_LATCHES.PC = Low16bits(NEXT_LATCHES.PC + (SEXT9(pc_offset) << 1)); // since we alr incremented pc
  }
  // else pc has already been incremented, we r good
}

void execute_jump(int base_register){// jmp or ret. if ret, then base reg will =7
  NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.REGS[base_register]);
}

void execute_jsr(int arg1, int flag){
  /*
  In Appendix A, please correct the operation of the JSR/JSRR instruction to read:
	TEMP = PC†
	if (bit(11)==0)
	    PC = BaseR;
	else
	    PC = PC† + LSHF(SEXT(PCoffset11), 1);
	R7 = TEMP;
	
	where PC†: incremented PC
*/
int temp = Low16bits(NEXT_LATCHES.PC);
  if(flag){ // jsr
    NEXT_LATCHES.PC = Low16bits(NEXT_LATCHES.PC + (SEXT11(arg1) << 1)); // arg1 = pcoffset11, alr incremented pc in fetch 
  } else { // jsrr
    NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.REGS[arg1]); // arg1 = base reg
  }
  NEXT_LATCHES.REGS[7] = temp; // so you can jump back
}

void execute_ldb(int dr, int baser, int offset){
  // DR = SEXT(mem[BaseR + SEXT(boffset6)]);
  int addr = Low16bits(CURRENT_LATCHES.REGS[baser] + SEXT6(offset)); 
  int val = MEMORY[addr >> 1][addr & 0x01]; 
  // addr/2 bc memory is word addressable and then lsb of addr chooses which byte of the word
  val = SEXT8(val); // sign extend the byte to 16 bits
  NEXT_LATCHES.REGS[dr] = val;
  update_cc(val);
}

void execute_ldw(int dr, int baser, int offset){
  int addr = Low16bits(CURRENT_LATCHES.REGS[baser] + SEXT6(offset));
  int val = ((MEMORY[addr >> 1][1] & 0xFF) << 8) | (MEMORY[addr >> 1][0] & 0xFF); // get both bytes
  NEXT_LATCHES.REGS[dr] = val;
  update_cc(val);
}

void execute_lea(int dr, int pcoffset){
  // offset is memory addr, needs to x2
  int addr = Low16bits(CURRENT_LATCHES.PC + (SEXT9(pcoffset) << 1));
  NEXT_LATCHES.REGS[dr] = addr; // the dr gets an address not a value for lea
  // doc says lea does not set cc
}
void execute_xor(int dr, int sr1, int op2, int flag){
  int op1_val = CURRENT_LATCHES.REGS[sr1];
  int op2_val = flag ? SEXT5(op2) : CURRENT_LATCHES.REGS[op2]; // flag = 0 means reg mode
  // if not, then imm5= 11111
  int res = Low16bits(op1_val ^ op2_val);
  NEXT_LATCHES.REGS[dr] = res;
  update_cc(res);
}

void execute_shift(int dr, int sr, int amount, int flag){
  // flag = 0 lshf, 1 rshfl, 3 rshfa
  int val = CURRENT_LATCHES.REGS[sr];
  int res;
  if(flag == 0){ // lshf
    res = Low16bits(val << amount);
  } else if(flag == 1){ // rshfl
    res = Low16bits(val >> amount);
  } else { // rshfa
    // arithmetic shift right: preserve sign bit
    if(val & 0x8000){ // if msb = 1 val is negative
      res = (val >> amount) | (0xFFFF << (16 - amount)); // fill in with 1s on the left empty sides
    } else {
      res = val >> amount;
    }
    res = Low16bits(res);
  }
  NEXT_LATCHES.REGS[dr] = res;
  update_cc(res);
}

void execute_stb(int sr, int baser, int offset){
  int addr = Low16bits(CURRENT_LATCHES.REGS[baser] + SEXT6(offset));
  int val = CURRENT_LATCHES.REGS[sr] & 0xFF; // get lower BYTE
  MEMORY[addr >> 1][addr & 0x01] = val; 
  // addr/2 bc memory is word addressable and then lsb of addr chooses which byte of the word to store into
}

void execute_stw(int sr, int baser, int offset){
  int addr = Low16bits(CURRENT_LATCHES.REGS[baser] + SEXT6(offset));
  int val = CURRENT_LATCHES.REGS[sr];
  MEMORY[addr >> 1][0] = val & 0xFF; // store lower byte
  MEMORY[addr >> 1][1] = (val >> 8) & 0xFF; // store upper byte
}

void execute_trap(int trap_vector){
  NEXT_LATCHES.REGS[7] = Low16bits(NEXT_LATCHES.PC); // load with incremented pc
  // trap vector is 8 bits, gives addr of routine 
  int vector_table_addr = SEXT8(trap_vector) << 1; // multiply by 2 to get word address
  int routine_addr = ((MEMORY[vector_table_addr >> 1][1] & 0xFF) << 8) | (MEMORY[vector_table_addr >> 1][0] & 0xFF); // get both bytes
  NEXT_LATCHES.PC = Low16bits(routine_addr); // jump to routine
  update_cc(NEXT_LATCHES.PC); // update cc based on new pc33

}
