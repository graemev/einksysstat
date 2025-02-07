#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>

#include <stdint.h>
#include <assert.h>
#include <sys/param.h>





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

float mail_vcore(char request[],
		 char response[],
		 int  max_response)
{
  int			mbfd;  
  union msg_format	msg;
  int			len;
  int			rc;

  assert(sizeof (as_array_t) == sizeof (as_struct_t));  // Must line up with the 260 words.
  
  /* Assemble the outgoing message */
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

