#include "ServiceQueueControlTask.h"

void ServiceQueueControlTask(void *param_struct)
{
  ServiceQueueControl_parameter *parameters_for_you;
  parameters_for_you = (ServiceQueueControl_parameter *) param_struct;
  service_req req_in;
  service_req temp_req;
  service_req current_working_req;
  int emergancy_state_var = 0;
  QueueHandle_t service_queue_var = xQueueCreate( UBaseType_t 50,
                                                UBaseType_t sizeof(service_req));
                                     // Change this to an internal enum maybe.

  int current_floor = 0;

  while (1)
  {

      if (uxQueueMessagesWaiting(
               parameters_for_you->m_service_request_message_queue))
      {
         // read the mail.
          xQueueReceive( parameters_for_you->m_incoming_led_queue,
                        &req_in,
                        0);
          // processes the mail.
          switch(req_in)
          {
                  case EmergStop:
                       vQueueDelete( service_queue_var);
                       emergancy_state_var = 1;
                       break;
                  case EmergClear:
                      vQueueDelete( service_queue_var);
                      emergancy_state_var = 0;
                      break;
              case DoorInterference:
                  // if we just got mail that there is door interferance
                  xQueuePeek(service_queue_var, temp_req, 0);

                  if (temp_req == CloseDoor) // and we are trying to close the door.
                  {
                      current_working_req = OpenDoor; // open it instead.
                      xQueueSendToFront(service_queue_var, current_working_req, 0);
                  }
                  else if (temp_req == OpenDoor) // or if we are trying to open the door.
                  {
                      current_working_req = CloseDoor; // close it instead.
                      xQueueSendToFront(service_queue_var, current_working_req, 0);
                  }
                  break;
          }

      }




  }


}

