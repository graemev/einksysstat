#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>

#include <stdint.h>
#include <assert.h>
#include <sys/param.h>

#define FAN     "/sys/devices/platform/cooling_fan/hwmon/hwmon3/fan1_input"
#define CPUTEMP "/sys/devices/virtual/thermal/thermal_zone0/temp"

#define DEVICE_FILE_NAME "/dev/vcio"
#define MAJOR_NUM 100
#define IOCTL_MBOX_PROPERTY _IOWR(MAJOR_NUM, 0, char *)
#define GET_GENCMD_RESULT 0x00030080




/*
 * We are dealing with an ABI (not API), so the actula bit patterns matter. So we have an array of unsigned 32 bit words.
 * we can't just uint (or similar) and hope , the elements (words) must be 4 bytes long. C defines a char as 1 byte so that is OK
 * Additionally , while I belive the ABI allows some flexibility about buffer size (provided it's defined) the vcore end of
 * this interface seems to send back verbose human readable string (e.g. error messages) so I'm going to guess there is an assumption
 * that the client is vcgencmd(1) which has a 1024 byte "buffer" to hold the string and the response.
 */

#define  STRING_SIZE	 1024  /* Bytes */
#define  HEADER_OVERHEAD 7     /* words */


typedef uint32_t  as_array_t[STRING_SIZE/4+HEADER_OVERHEAD];
typedef struct {

  uint32_t  size;
  uint32_t  request;
  uint32_t  tag_id;
  uint32_t  buf_len;
  uint32_t  resp_len;
  uint32_t  resp_code;
  char      string[STRING_SIZE];	// string version of command starts here.
  uint32_t  end_tag;;

} as_struct_t;


union msg_format {
  as_array_t	a;
  as_struct_t	s;
};

static float mail_vcore(char request[],
			char response[],
			int  max_response)
{
  int			mbfd;  
  union msg_format	msg;
  int			len;
  int			rc;

  /* Assemble the outgoing message*/
  msg.s.size      = STRING_SIZE+HEADER_OVERHEAD*4;
  msg.s.request   = 0x00000000;		// process request
  msg.s.tag_id    = GET_GENCMD_RESULT;	// Undefined Magic
  msg.s.buf_len   = STRING_SIZE;	// request_len on send 
  msg.s.resp_len  = 0;			// indicates response length on return
  msg.s.resp_code = 0;			// Set to zero now, so non-zero was error set at the other end

  strncpy(msg.s.string, request, STRING_SIZE);

  msg.s.end_tag  = 0x00000000;		// Always an zero word



  
  // Magic char device file used for communicating with kernel mbox driver (via ioctl calls)
  if ((mbfd = open(DEVICE_FILE_NAME, 0)) < 0)
    {
      perror(DEVICE_FILE_NAME);
      exit(-1);
    }

  /* Send the message */
  if ((rc = ioctl(mbfd, IOCTL_MBOX_PROPERTY, msg.a)) < 0)  // send the cmd to the videocore
      perror("ioctl to MBOX");

  close(mbfd);

  rc = msg.s.resp_code;  // any error code

  len = MIN(max_response, msg.s.resp_len);

  strncpy(response, msg.s.string, len);

  return rc;
}



static float get_gputemp2()
{
  char *send="measure_temp";
  char receive[STRING_SIZE];
  int  rc;
  float gputemp;
  char  scale;

  if ((rc=mail_vcore(send, receive, STRING_SIZE)) !=0)
      fprintf(stderr, "Non-Zero code (%d) from measure temp request\n", rc);
  
  fprintf(stderr, "get_gputemp2 result was %s\n", receive);
  
  if (sscanf(receive, " temp=%f'%c", &gputemp, &scale) != 2)    // eg "temp=53.8'C"
    fprintf(stderr, "Bad data in response from MBOX\n");

  fprintf(stderr, "gputemp=%f, scale=%c\n", gputemp, scale);

  return gputemp;
}




static int get_throttled()
{
  char *send="get_throttled";
  char receive[STRING_SIZE];
  int  rc;
  int  throttled;


  if ((rc=mail_vcore(send, receive, STRING_SIZE)) !=0)
      fprintf(stderr, "Non-Zero code (%d) from measure temp request\n", rc);

  fprintf(stderr, "get_throttled result was %s\n", receive);

  if (sscanf(receive, " throttled=%x", &throttled) != 1)    // e.g. throttled=0x0
    fprintf(stderr, "Bad data in response from MBOX\n");
 
  
  return throttled;
}





int main(int argc, char *argv[])
{
  char   *line = NULL;
  size_t  len = 0;
  ssize_t nread;
  int	  fanspeed;
  int	  cputemp;
  float	  gputemp;
  int	  throttled;
  
  assert(sizeof (as_array_t) == sizeof (as_struct_t));

  
  FILE *fp;

  if ((fp=fopen(FAN, "r")) == NULL)
    {
      perror(FAN);
      exit(1);
    }

  if ((nread = getline(&line, &len, fp)) > 0)
    {
      printf("Fan says %s\n", line);
      if (sscanf(line, "%d", &fanspeed) != 1)
	fprintf(stderr, "Bad data in %s\n", FAN);
      printf("That's a fan speed of %d\n", fanspeed);
    }
  
  fclose(fp);
  

  if ((fp=fopen(CPUTEMP, "r")) == NULL)
    {
      perror(CPUTEMP);
      exit(1);
    }

  if ((nread = getline(&line, &len, fp)) > 0)
    {
      printf("temp says %s\n", line);
      if (sscanf(line, "%d", &cputemp) != 1)
	fprintf(stderr, "Bad data in %s\n", CPUTEMP);
      printf("That's a CPU temperature of %5.2f degrees C\n", (cputemp/1000.0));
    }



  gputemp=get_gputemp2();

  printf("GPU temperature of %6.2f degrees C\n", gputemp);

  throttled=get_throttled();

  printf("Throttle status = %#08X\n", throttled);

  
  fclose(fp);
  
  free(line);





  
  


  
}
