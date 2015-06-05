#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>


#define FILE_ERROR(fp,error_msg) 	do { \
					  if ((fp)==NULL) { \
					    printf("%s\n",(error_msg)); \
					    exit(EXIT_FAILURE); \
					  }	\
					} while(0)

#define NUM_MEASUREMENTS 50
#define SLEEP_TIME	30	// every SLEEP_TIME seconds, sensors wake up and measure temperature
#define NUM_SENSORS	8	// also defines number of rooms (currently 1 room = 1 sensor)
#define TEMP_DEV 	5	// max afwijking vorige temperatuur in 0.1 celsius
#define SET_MIN_TEMP	17	// generate log event if running avg becomes smaller
#define SET_MAX_TEMP	24	// generate log event if running avg becomes bigger
#define NUM_AVG		5

#define MAP_FILE room_sensor.map
#define DATA_FILE sensor_data
#define DATA_FILE_TEXT sensor_data_text  // only for debugging
#define LOG_FILE log

uint16_t room_id[NUM_SENSORS] = {1,2,3,4,11,12,13,14};
uint16_t sensor_id[NUM_SENSORS] = {1,2,3,4,5,6,7,8};
double sensor_temperature[NUM_SENSORS][NUM_AVG] = {{15},{17},{18},{19},{20},{23},{24},{25}}; // starting temperatures
	// 2D-array: [NUM_AVG] is used as a circular array that contains the latest temperatures to compute the running avg

int main( int argc, char *argv[] )
{
  FILE *fp_text, *fp_bin, *fp_log;
  int i, j, k, index, next_index;
  double avg;
  time_t starttime = time(&starttime);
  srand48( time(NULL) );
  
  // generate ascii file room_sensor.map
  fp_text = fopen(MAP_FILE, "w");
  FILE_ERROR(fp_text,"Couldn't create room_sensor.map\n");
  for( i=0; i<NUM_SENSORS; i++)
  {
    fprintf(fp_text,"%" PRIu16 " %" PRIu16 "\n", room_id[i], sensor_id[i]); 
  }
  fclose(fp_text);
 
  // generate binary file sensor_data and corresponding log file
  fp_bin = fopen(DATA_FILE, "w");
  FILE_ERROR(fp_bin,"Couldn't create sensor_data\n");  
  fp_log = fopen(LOG_FILE, "w");
  FILE_ERROR(fp_log,"Couldn't create log\n");
  #ifdef DEBUG // save sensor data also in text format for test purposes
    fp_text = fopen(DATA_FILE_TEXT, "w");
    FILE_ERROR(fp_text,"Couldn't create sensor_data in text\n");
  #endif
  for( i=0; i<NUM_MEASUREMENTS; i++, starttime+=SLEEP_TIME ) 
  {
    index = i % NUM_AVG; // current index in circular array
    next_index = (i+1) % NUM_AVG; // index of new temperature value in circular array
    for( j=0; j<NUM_SENSORS; j++)
    {
       // write current temperatures to file
      fwrite(sensor_id+j, sizeof(sensor_id[0]), 1, fp_bin);
      fwrite(&(sensor_temperature[j][index]), sizeof(sensor_temperature[0][0]), 1, fp_bin);
      fwrite(&starttime, sizeof(time_t), 1, fp_bin);      
      #ifdef DEBUG
	fprintf(fp_text,"%" PRIu16 " %g %ld\n", sensor_id[j],sensor_temperature[j][index],(long)starttime);    
      #endif
      
      if (i >= NUM_AVG) // start computing running avg
      {
        // brute-force computation of running avg 
        for( k=0, avg = 0; k<NUM_AVG ; k++)
	{
	  avg+=sensor_temperature[j][k];
	}
	avg /= NUM_AVG;
	
        // check avg boundaries
        if (avg < SET_MIN_TEMP)
        {
	  fprintf(fp_log,"Room %" PRIu16 " is too cold\n", room_id[j]);
        }
        if (avg > SET_MAX_TEMP)
        {
	  fprintf(fp_log,"Room %" PRIu16 " is too hot\n", room_id[j]);
        }
      }
      
      // get new temperature: still needs some fine-tuning ...
      sensor_temperature[j][next_index] = sensor_temperature[j][index] + TEMP_DEV * ((drand48() - 0.5)/10); 
    }
  }
  fclose(fp_bin);
  fclose(fp_log);
  #ifdef DEBUG
    fclose(fp_text);
  #endif
  
  return 0;
}

