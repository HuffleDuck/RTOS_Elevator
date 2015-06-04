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


  MotorMessage motor_message_to_send;
  // all possible motor messages
  char motor_msg_default[15] = "Yoooo...";
  motor_msg_default[15] = 0x00;


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
  int math_temp = 0;

  bool new_service = true;

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
                      case CallToGNDInsideCar:
                          xQueueSendToBack(service_queue_var, (void*) &req_in, 0 );
                         break;
                      case CallToP1InsideCar:
                          xQueueSendToBack(service_queue_var, (void*) &req_in, 0 );
                          break;
                      case CallToP2InsideCar:
                          xQueueSendToBack(service_queue_var, (void*) &req_in, 0 );
                          break;
                      case CallToGNDOutsideCar:
                          xQueueSendToBack(service_queue_var, (void*) &req_in, 0 );
                          break;
                      case CallToP1fromOutsideCar:
                          xQueueSendToBack(service_queue_var, (void*) &req_in, 0 );
                          break;
                      case CallToP2fromOutsideCar:
                          xQueueSendToBack(service_queue_var, (void*) &req_in, 0 );
                          break;
                      case OpenDoor:
                          xQueueSendToBack(service_queue_var, (void*) &req_in, 0 );
                          break;
                      case CloseDoor:
                          xQueueSendToBack(service_queue_var, (void*) &req_in, 0 );
                          break;
                      case ChangeMaxSpeedToN: // We will actually update this after we
                                            // are done with the services already queued.
                          // No need to change this while the car is in motion.
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

                switch(current_working_req)
                {
                    case CallToGNDInsideCar:
                        int distance_to_top_speed, acel_and_decel_distance = 0;
                        int tot_travel_distnace = 0;
                        int cruise_distance = 0;
                        if (new_service == true)
                        {
                            new_service = false; // only send the message once.
                            // Check to see if the door is busy.
                            //  vf^2/2a = d
                            // math temp holds curent distance to reach top speed.
                            distance_to_top_speed =  current_max_speed * current_max_speed / (2 * current_acel);
                            acel_and_decel_distance = distance_to_top_speed * 2;
                            // What is our current floor?
                            cruise_distance =  (DISTNACE_FROM_GND_TO_P1 acel_and_decel_distance)

                            strcpy(motor_message_to_send.state, motor_msg_default);
                            motor_message_to_send.m_time_to_spend_in_accel = 5;
                            motor_message_to_send.m_time_to_spend_in_cruise = 5;
                            motor_message_to_send.m_time_to_spend_in_decel = 5;
                            motor_message_to_send.m_emer_flag = false;
                            motor_message_to_send.m_emer_flag = false;


                        }
                        if (xSemaphoreTake(parameters_for_you->m_service_done, 0))
                        {
                           // So only remove this from the front of the queue
                            // when the motor signals that it is done.
                            xQueueReceive( service_queue_var,
                                                        &temp_req,
                                                        0); // throw temp_req away.
                            new_service = true;
                        }
                        break;
                    default: 
                        break;



                }
//
//                fake_time_spent_for_testing++;
//                if (fake_time_spent_for_testing > 500)
//                {
//                    xQueueReceive( service_queue_var,
//                                     (void*) &current_working_req,
//                                0);
//                    fake_time_spent_for_testing = 0;
//                }
          }
      }
  }


}

