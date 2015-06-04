#include "ServiceQueueControlTask.h"
///////////////////////////////////////
// Author: Gabriel McDermott
// Purpose: Takes in requests for elevator service over a message queue.
// Services those requests one at a time, sending control messages to the motor
// and door tasks. 
// Created: 6/2/15
// Modified: 6/3/15
//////////////////////////////////////
void ServiceQueueControlTask(void *param_struct)
{
  ServiceQueueControl_parameter *parameters_for_you;
  parameters_for_you = (ServiceQueueControl_parameter *) param_struct;
  service_req req_in;
  service_req temp_req;
  service_req current_working_req;
  int emergancy_state_var = 0;

  char emergancy_lockout_message[81] = "Emergency Lockdown in Progress.\r\n"
                                    "Clear Emergancy Lockdown to Resume Operation.\r\n";
  char door_interferance_message[9] = "Sorry.\r\n";
  char garbage_mail_message[34] = "ServiceQueue received bad input.\r\n";
  char emergancy_set_message[21] = "Emergency Set./////\r\n";
  char emergancy_clear_message[23] = "Emergency clear./////\r\n";
  char service_queue_boot_message[34] = "//////Elevator booting//////////\r\n";
  emergancy_lockout_message[81] = 0x00; // null terminate.
  door_interferance_message[9] = 0x00;
  service_queue_boot_message[34] = 0x00;
  garbage_mail_message[34] = 0x00;
  emergancy_set_message[21] = 0x00;
  emergancy_clear_message[23] = 0x00;
  QueueHandle_t service_queue_var = xQueueCreate( 50, sizeof(service_req));
                                     // Change this to an internal enum maybe.

  int current_floor = 0;
  int current_max_speed = 20;
  int current_acel = 2;

  int fake_time_spent_for_testing = 0;
  UartMessageOut(&service_queue_boot_message);
  while (1)
  {

      if (uxQueueMessagesWaiting(
               parameters_for_you->m_service_request_message_queue))
      {
         // read the mail.
          xQueueReceive( parameters_for_you->m_service_request_message_queue,
                        &req_in,
                        0);
          // processes the mail.///////

          // if mail is that there is an emergancy or emergancy clear.
          if (req_in == EmergStop)
          {
                vQueueDelete( service_queue_var);
                emergancy_state_var = 1;
                 UartMessageOut(&emergancy_set_message);
          }
          else if ( req_in == EmergClear)
          {
              emergancy_state_var = 0;
              UartMessageOut(&emergancy_clear_message);
          } else
          {

              if (emergancy_state_var == 0)
              {  // Only queue mail if we are not in an emergancy.
                  switch(req_in)
                  {
                     case DoorInterference:
                          // if we just got mail that there is door interferance
                          xQueuePeek(service_queue_var, (void*) &temp_req, 0);
                          UartMessageOut(& door_interferance_message);
                          if (temp_req == CloseDoor) // and we are trying to close the door.
                          {
                              // Stop trying to open it.
                            xQueueReceive( service_queue_var,
                                                    &temp_req,
                                                    0); // throw temp_req away.
                              current_working_req = OpenDoor; // open it instead.

                              xQueueSendToFront(service_queue_var,  // like, now.
                                               (void *) &current_working_req, 0);
                          }
                          else if (temp_req == OpenDoor) // or if we are trying to open the door.
                          {
                                                        // Stop trying to open it.
                            xQueueReceive( service_queue_var,
                                                        &temp_req,
                                                        0); // throw temp_req away.

                              current_working_req = CloseDoor; // close it instead.
                              xQueueSendToFront(service_queue_var,
                                                (void *) &current_working_req, 0);
                          }
                          else
                          { // If the thing we are doing right now is something
                              // other than opening or closing the door, and we
                              // are getting a door interferance signal, trigger
                              // an emergancy state.
                              UartMessageOut(&emergancy_set_message);
                              vQueueDelete( service_queue_var);
                              emergancy_state_var = 1;
                          }
                          break;




                       // Essentially all defaults. Queue without further modification.
                      case OpenDoor:
                          xQueueSendToBack(service_queue_var, (void*) &req_in, 0 );
                          break;
                      case CloseDoor:
                          xQueueSendToBack(service_queue_var, (void*) &req_in, 0 );
                          break;
                      case ChangeMaxSpeedToN: // We will actually update this after we
                                            // are done with the services already queued.
                          xQueueSendToBack(service_queue_var, (void*) &req_in, 0 );
                       case ChangeMaxAccelToN:
                          xQueueSendToBack(service_queue_var, (void*) &req_in, 0 );
                      default: // The message you just sent me does not make sense.
                          // Send a message to UART_TX here informing the user that
                          // ServiceQueueControl got a garbage message.

                          UartMessageOut(&garbage_mail_message);
                          break;
                  }
              }
              else
              {
                  UartMessageOut(&emergancy_lockout_message);
              }

          }
      }

      /// Now that all the incoming mail has been properly queued,

      // if we are not in an emergancy.
      if (emergancy_state_var == 0)
      {
      // lets do the thing at the front of the service queue.

          if (uxQueueMessagesWaiting(service_queue_var) > 0)
          {
                xQueuePeek(service_queue_var, (void*) &current_working_req, 0);
          // For now, pretend to work, and then take the thing at the front of the
          //  service queue and throw it away.

                fake_time_spent_for_testing++;
                if (fake_time_spent_for_testing > 500)
                {
                    xQueueReceive( service_queue_var,
                                     (void*) &current_working_req,
                                0);
                    fake_time_spent_for_testing = 0;
                }
          }
      }
  }


}

