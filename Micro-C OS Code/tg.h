  #include "alt_types.h"
  #include "altera_avalon_pio_regs.h"
  #include "alt_ucosii_simple_error_check.h"

  #include <stdio.h>
  #include <system.h>
  #include <math.h>
  #include <stdlib.h>  
  #include <sys/time.h>
  #include <sys/alt_timestamp.h>

  
  #define REGIONS 8

// Stack declarations for the program
  #define TASK_STACKSIZE 2049 
  
  // These are the priorities of the functions
  #define USER_PRIORITY                      7
  #define SOFTWARE_FUNCTION_PRIORITY         6

  // The stacks
  OS_STK    user_stk[TASK_STACKSIZE];
  OS_STK    software_function_stk[TASK_STACKSIZE];

  // For the generator
  // The amount of regions is the length of the array.
  int region_bounds[REGIONS];
  int rnd = 3456;  
  int tracker = 0;
  // http://www.augustana.ab.ca/~mohrj/algorithms/scramble.html for new lists of random numbers
  int random_array[101] = { 63, 
                            81, 
                            57, 
                            45, 
                            61,
                            60,
                            21,
                            53,
                            30,
                            39,
                            86,
                            9,
                            12,
                            34,
                            4,
                            56,
                            33,
                            15,
                            42,
                            24,
                            28,
                            11,
                            68,
                            43,
                            37,
                            75,
                            47,
                            59,
                            26,
                            1,
                            65,
                            36,
                            79,
                            51,
                            8,
                            90,
                            66,
                            69,
                            88,
                            89,
                            74,
                            52,
                            71,
                            95,
                            41,
                            97,
                            18,
                            22,
                            13,
                            3,
                            44,
                            62,
                            16,
                            20,
                            29,
                            49,
                            35,
                            76,
                            7,
                            92,
                            96,
                            87,
                            93,
                            84,
                            38,
                            78,
                            40,
                            25,
                            72,
                            23,
                            70,
                            77,
                            83,
                            10,
                            5,
                            73,
                            58,
                            6,
                            101,
                            31,
                            100,
                            50,
                            98,
                            80,
                            32,
                            19,
                            67,
                            54,
                            64,
                            48,
                            17,
                            55,
                            46,
                            91,
                            14,
                            27,
                            94,
                            99,
                            82,
                            85,
                            2};
    
  
  int region;
  // This tells if the function is enabled or not
  int enable = 0;
  int ans;
  
  // This is for the random number generator function
  float rand_func(int);
  
  // Task prototypes
  void software_function(void* pdata);
  void user(void* pdata)  ;
  void generator_options();
  void analyser_options() ;  
  void get_regions()  ;
  int generate_destination();
  int get_number()    ;
  char get_yn()       ;
  char get_ga()       ;

  // For the analyser
  // Variables
  static alt_u8 port,tport;

  // Prototypes
  int testHisto();
  void printPort(int);
  void print_stats();
  char get_gp();