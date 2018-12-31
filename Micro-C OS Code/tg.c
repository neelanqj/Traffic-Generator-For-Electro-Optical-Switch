#include "tg.h"

int main(void)
{
  // Initializing the OS internals
  OSInit();
  
  //this part simply creates the individual task
  OSTaskCreateExt(software_function,
                  NULL,
                  (void *)&software_function_stk[TASK_STACKSIZE -1],
                  SOFTWARE_FUNCTION_PRIORITY,
                  SOFTWARE_FUNCTION_PRIORITY,
                  software_function_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);
                  
  OSTaskCreateExt(user,
                  NULL,
                  (void *)&user_stk[TASK_STACKSIZE -1],
                  USER_PRIORITY,
                  USER_PRIORITY,
                  user_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0); 
  OSStart();
  return 0; 
}

/*  ***************************************************************************
 *  ***************************************************************************
 *  ******** These are the tasks that will continuously be running ************
 *  ************************ in the background. *******************************
 *  ***************************************************************************
 *  ************************************************************************* */
  void user(void* pdata){
      /* ***********************************************************************
       * The data needed from the user is as follows:
       *   0. Whether to access the traffic analyser or traffic generator.
       *   1. To disable of enable the traffic generator (send and enable).
       *   2. Whether to bias a specific destination or not.
       *   3. Set the amont of traffic that wil be sent out.
       *   4. Set the amount of traffic that will be sent to certain destinations.
       *   5. Set whether to use software send/software based destination chooser.
       *   6. Set the software destinations.
       * ********************************************************************* */       
      int answer;
      
      printf("Started PGA.\n\r");
      // Pre-running setup options. (Stops processes that were running beforehand)                       
      while (1){
              generator_options();          
              OSTimeDlyHMSM(0, 0, 1, 0);
               }
      }
  
    void software_function(void* pdata){
      /* ***********************************************************************
       * This function is the software function that is used in the case that 
       * the user desires a special distribution. This distribution should be 
       * able to generate almost any type of distribution.
       * ********************************************************************* */
      while (1){
        switch (enable){
          case 0: 
          // When a software function is not enabled, nothing will happen.           
            break;
          case 1:
          // ----------------------------------------------------------------
          // The non-uniform traffic generator
          // This will enable the sending of packets on every line.            
            IOWR_ALTERA_AVALON_PIO_DATA(PG_CD0_BASE, generate_destination());
            IOWR_ALTERA_AVALON_PIO_DATA(PG_CD1_BASE, generate_destination());
            IOWR_ALTERA_AVALON_PIO_DATA(PG_CD2_BASE, generate_destination());
            IOWR_ALTERA_AVALON_PIO_DATA(PG_CD3_BASE, generate_destination());
            IOWR_ALTERA_AVALON_PIO_DATA(PG_CD4_BASE, generate_destination());
            IOWR_ALTERA_AVALON_PIO_DATA(PG_CD5_BASE, generate_destination());
            IOWR_ALTERA_AVALON_PIO_DATA(PG_CD6_BASE, generate_destination());
            IOWR_ALTERA_AVALON_PIO_DATA(PG_CD7_BASE, generate_destination());
          // ----------------------------------------------------------------
            break;
          default:
            break;
          }
        OSTimeDlyHMSM(0, 0, 0, 5);
        }
      }  

/*  ***************************************************************************
 *  ********* These are functions, made to make the code more readable ********
 *  ************************************************************************* */
void generator_options(){
       // Variable declarations.
       int load                     = 0   ;
       int destination_bias         = 0   ;
       int destination_to_be_biased = 0   ;
       int distribution             = 255 ;
       int answer                         ;
      /* ***********************************************************************
       * The data needed from the user is as follows:
       *   1. To disable of enable the traffic generator (send and enable).
       *   2. Whether to bias a specific destination or not.
       *   3. Set the amont of traffic that wil be sent out.
       *   4. Set whether to use software send/software based destination chooser.
       *   5. Set the software destinations.
       * ********************************************************************* */              
       
       // This will get the inputs from the user
       printf("Please enter the load [range: 0 - 127]\n\r");
       load = get_number();
       
       printf("Do you wish to use a custom software distribution?\n\r");
       answer = (int)get_yn();
       if (answer == (int)'y' || answer == (int)'Y') {
         // This will be run if a custom destination is chosen.
         // Put the setup parameters here.
         get_regions(); // Gets the regions.
         
         enable = 1;    // Starts the software generator.
         IOWR_ALTERA_AVALON_PIO_DATA(PG_ENABLE_CUSTOM_DESTINATION_BASE, 255);
         }
       else {
         enable = 0; // Stops the software generator
         IOWR_ALTERA_AVALON_PIO_DATA(PG_ENABLE_CUSTOM_DESTINATION_BASE, 0);
         // Gets the distribution type.
         printf("Please enter the distribution method (0 for pareto, and 255 for non-uniform)\n\r");
         distribution = get_number();         

         //load = (1 - load) * 127 * 101 / (load * 10000);

         // This will be run if the default distribution is run.
         printf("Please enter the destination to be biased [range: 0 - 15]\n\r");
         destination_to_be_biased = get_number();
         
         printf("Please enter the load allocated to the specified destination [range: 0 - 127]\n\r");
         destination_bias = get_number();
         }
            
       printf("Start the packet generation? (Y for yes, N for no)\n\r");
       answer = get_yn();
       
      // Sets up the parameters.
      IOWR_ALTERA_AVALON_PIO_DATA(PG_BIAS_OF_DESTINATION_BASE, destination_bias);
      IOWR_ALTERA_AVALON_PIO_DATA(PG_BIASED_DESTINATION_BASE, destination_to_be_biased);
      IOWR_ALTERA_AVALON_PIO_DATA(PG_BIAS_OF_LOAD_BASE, load);
      IOWR_ALTERA_AVALON_PIO_DATA(PG_PARETO_OR_RANDOM_BASE, distribution);
   
       // This will get an answer.
       if (answer == (int)'y' || answer == (int)'Y') {
         IOWR_ALTERA_AVALON_PIO_DATA(PG_ENABLE_PACKET_SENDING_BASE, 255);
         }
       else {
         IOWR_ALTERA_AVALON_PIO_DATA(PG_ENABLE_PACKET_SENDING_BASE, 0);
         }
  }

/* ************************************************************************************
 * ******************** The lower level functions are listed here. ********************
 * ********************************************************************************** */
// For the custom random number generator.
int generate_destination(){
  
    rnd = random_array[tracker];          
    tracker = (tracker + 3) % 101;
    
    if (rnd < region_bounds[0])
       region = 0;
    else if  (rnd < region_bounds[1])
       region = 1;
    else if  (rnd < region_bounds[2])
       region = 2;
    else if  (rnd < region_bounds[3])
       region = 3;
    else if  (rnd < region_bounds[4])
       region = 4;
    else if  (rnd < region_bounds[5])
       region = 5;
    else if  (rnd < region_bounds[6])
       region = 6;
    else
       region = 7;

    return region;
  }

/*  ***************************************************************************
 *  **************** These functions get inputs and check them ****************
 *  ************************************************************************* */
int get_number(){
    /* ************************************************************************
     * * This function will get an integer character input and output it as a *
     * * single integer. **************************************************** *
     * ********************************************************************** */
    char character;
    int current_number = 0;
    
    while(1) {
      character = getchar();
        //Number conversions
      if ((int)character < 58 && (int)character > 47){
         current_number = current_number * 10 + (character - 48)       ;
        } 
      else if (character == '\n') {               
               break                                                    ;
        } 
      else if (character == '\r'){
               printf("Number added.\n\r")                              ;
        }
      else {
               printf("Invalid input, please re-enter.\n\r")            ;
               current_number = 0;
        }
      }
   return current_number;
  }

char get_ga(){
    /* *************************************************************************
     * This will accept only G or A as inputs, otherwise it will ask for the **
     * inputs again. *********************************************************** 
     * *********************************************************************** */
    ans = getchar();
    
    while(1) {
        if((int)ans == (int)'g' || (int)ans == (int)'G' || (int)ans == (int)'a' || (int)ans == (int)'A') {
          printf("\n\rValid answer.\n\r");
          break;
          }
        else {
          printf("\n\rInvalid answer, please enter again (G  or A).\n\r");
          ans = getchar();
          }
      }
    return ans;    
  }

char get_yn(){
  /* *************************************************************************
   * This will accept only Y or N as inputs, otherwise it will ask for the ***
   * inputs again ************************************************************ 
   * *********************************************************************** */
  ans = getchar();
  
  while(1) {
        if((int)ans == (int)'y' || (int)ans == (int)'Y' || (int)ans == (int)'n' || (int)ans == (int)'N') {
          printf("\n\rValid answer\n\r");
          break;
          }
        else {
          printf("\n\rInvalid answer, please enter again (Y for yes or N for no)\n\r");
          ans = getchar();
          }
      }
  return ans;
  }


char get_gp(){
  /* *************************************************************************
   * This will accept only G or P as inputs, otherwise it will ask for the ***
   * inputs again ************************************************************ 
   * *********************************************************************** */
  ans = getchar();
  
  while(1) {
        if((int)ans == (int)'g' || (int)ans == (int)'G' || (int)ans == (int)'p' || (int)ans == (int)'P') {
          printf("\n\rValid answer\n\r");
          break;
          }
        else {
          printf("\n\rInvalid answer, please enter again\n\r");
          ans = getchar();
          }
      }
  return ans;
  }

void get_regions(){
   /* *************************************************************************
    * ****** This is for obtaining the software generation parameters. ********
    * *********************************************************************** */
   int i = 0;

   printf("Please enter the regions for the distribution \n\r");
   // This will obtain the regions from the user
   for (i = 0; i < REGIONS ; i++){ 
           region_bounds[i] = get_number();
           if (region_bounds[i -1] > region_bounds[i]){
               printf("Invalid region bound, please re-enter region within bounds.\n\r");
               i --;
               }
      }
   // This displays the regions input by the user.
   for (i = 0; i < REGIONS ; i++)
       printf("|region -> %d, %d | ", i, region_bounds[i]);
   printf("\n\r");
  }
  