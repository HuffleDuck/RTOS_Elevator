#include "ServiceQueueControlTask.h"


 QueueHandle_t service_request_message_queue;
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
  ServiceQueueMessage req_in;
  ServiceQueueMessage temp_req;
  ServiceQueueMessage current_working_req;
  MotorMessage motor_message_to_send;



  int emergancy_state_var = 0;

  service_request_message_queue = xQueueCreate(20, sizeof(ServiceQueueMessage));
  QueueHandle_t service_queue_var = xQueueCreate( 50, sizeof(ServiceQueueMessage));
                                     // Change this to an internal enum maybe.

  volatile int current_floor = 0;
  float current_max_speed = 20;
  float current_acel = 2;

    bool new_service = true;

    char service_queue_boot_message[35] = "//////Elevator booting//////////\r\n";
    service_queue_boot_message[35] = 0x00; // Null terminate
    UartMessageOut(service_queue_boot_message);

  //////////////////internal into updated strings////////////////////////////////
  char max_speed_update_msg[25] = "Maximum speed updated.\r\n";
  char accel_update_msg[23] = "Accleration updated.\r\n";
  char invalid_update_msg[31] = "Invalid speed/acel specefied\r\n";
  max_speed_update_msg[25] = 0x00;
  accel_update_msg[23] = 0x00;
  invalid_update_msg[31] = 0x00;

 /////////////////All Floor Service strings//////////////
  char floor_gnd_message[23] = "Ground Floor Reached.\r\n";
  char floor_p1_message[27] = "First Penthouse Reached.\r\n";
  char floor_p2_message[28] = "Second Penthouse Reached.\r\n";

  floor_gnd_message[23] = 0x00;
  floor_p1_message[27] = 0x00;
  floor_p2_message[28] = 0x00;
/////////////////All Mail Handling Strings///////////////////////////////////////////
    char emergancy_lockout_message[81] = "Emergency Lockdown in Progress.\r\n"
                                    "Clear Emergancy Lockdown to Resume Operation.\r\n";
      char door_interferance_message[9] = "Sorry.\r\n";
      char garbage_mail_message[34] = "ServiceQueue received bad input.\r\n";
      char emergancy_set_message[21] = "Emergency Set./////\r\n";
      char emergancy_clear_message[23] = "Emergency clear./////\r\n";

      emergancy_lockout_message[81] = 0x00;
      door_interferance_message[9] = 0x00;
      
      garbage_mail_message[34] = 0x00;
      emergancy_set_message[21] = 0x00;
      emergancy_clear_message[23] = 0x00;
      ////////////////////////////////////////////////////////////////////////
    while (xSemaphoreTake(parameters_for_you->m_service_done, 0)) // Clear out the semaphore

  while (1)
  {

      //HandleMail(); // All this is huge and gross. Consider dumping it all in
      // function calls.
//////////////////////////////////////////////////////////////////////////////////
       if (uxQueueMessagesWaiting(service_request_message_queue))
      {
         // read the mail.
          xQueueReceive( service_request_message_queue,
                        &req_in,
                        0);
          // processes the mail.///////

          // if mail is that there is an emergancy or emergancy clear.
          if (req_in.m_please_do_this == EmergStop)
          {
                vQueueDelete( service_queue_var);
                emergancy_state_var = 1;
                 UartMessageOut(emergancy_set_message);
          }
          else if ( req_in.m_please_do_this == EmergClear)
          {
              emergancy_state_var = 0;
              UartMessageOut(emergancy_clear_message);
          } else
          {

              if (emergancy_state_var == 0)
              {  // Only queue mail if we are not in an emergancy.
                  switch(req_in.m_please_do_this)
                  {
                     case DoorInterference:
                          // if we just got mail that there is door interferance
                          xQueuePeek(service_queue_var, (void*) &temp_req, 0);
                          UartMessageOut(door_interferance_message);
                          if (temp_req.m_please_do_this == CloseDoor) // and we are trying to close the door.
                          {
                              // Stop trying to open it.
                            xQueueReceive( service_queue_var,
                                                    &temp_req,
                                                    0); // throw temp_req away.
                              current_working_req.m_please_do_this = OpenDoor; // open it instead.

                              xQueueSendToFront(service_queue_var,  // like, now.
                                               (void *) &current_working_req, 0);
                          }
                          else if (temp_req.m_please_do_this == OpenDoor) // or if we are trying to open the door.
                          {
                                                        // Stop trying to open it.
                            xQueueReceive( service_queue_var,
                                                        &temp_req,
                                                        0); // throw temp_req away.

                              current_working_req.m_please_do_this = CloseDoor; // close it instead.
                              xQueueSendToFront(service_queue_var,
                                                (void *) &current_working_req, 0);
                          }
                          else
                          { // If the thing we are doing right now is something
                              // other than opening or closing the door, and we
                              // are getting a door interferance signal, trigger
                              // an emergancy state.
                              UartMessageOut(emergancy_set_message);
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
                          break;
                       case ChangeMaxAccelToN:
                          xQueueSendToBack(service_queue_var, (void*) &req_in, 0 );
                          break;
                      default: // The message you just sent me does not make sense.
                          // Send a message to UART_TX here informing the user that
                          // ServiceQueueControl got a garbage message.

                          UartMessageOut(garbage_mail_message);
                          break;
                  }
              }
              else
              {
                  UartMessageOut(emergancy_lockout_message);
              }

          }
      }

       // Done handling mail.
///////////////////////////////////////////////////////////////////////////////////
      /// Now that all the incoming mail has been properly queued,

      // if we are not in an emergancy.
      if (emergancy_state_var == 0)
      {
      // lets do the thing at the front of the service queue.

          if (uxQueueMessagesWaiting(service_queue_var) > 0)
          {
                xQueuePeek(service_queue_var, (void*) &current_working_req, 0);

                switch(current_working_req.m_please_do_this )
                {
                    case ChangeMaxSpeedToN:

                        if (new_service == true)
                        {
                            new_service = false;

                             current_max_speed = current_working_req.m_data;                       
                             SendMessageToMotor( 'S', 0, 0, 0, false, false, false, current_max_speed);
                        }
                         if (xSemaphoreTake(parameters_for_you->m_service_done, 0))
                        {

                             xQueueReceive( service_queue_var,
                                                        &temp_req,
                                                        0); // throw temp_req away.

                            UartMessageOut(max_speed_update_msg);

                            new_service = true;
                        }
                        break;

                   case ChangeMaxAccelToN:

                       if ( new_service == true)
                       {
                           new_service  = false;
                         current_acel = current_working_req.m_data;
                        SendMessageToMotor( 'A', 0, 0, 0, false, false, false, current_acel);
                       }
                       // Do not advance until the motor awknogledges the update.
                        xQueueReceive( service_queue_var,
                                                        &temp_req,
                                                        0); // throw temp_req away.

                        if (xSemaphoreTake(parameters_for_you->m_service_done, 0))
                        {
                           // So only remove this from the front of the queue
                            // when the motor signals that it is done.
                            xQueueReceive( service_queue_var,
                                                        &temp_req,
                                                        0); // throw temp_req away.
                            UartMessageOut(accel_update_msg);

                            new_service = true;
                        }
                        break;

                    case CallToP2InsideCar:
                        if (new_service == true)
                        {

                            new_service = false; // only send the message once.
                                            // we will then wait and check mail
                                           // until the door signals its done.
                            motor_message_to_send = CreateNewMotorMessage(current_max_speed,
                                                                            current_acel,
                                                                            current_floor,
                                                                                      2);

                            motor_message_to_send.m_emer_flag = false;
                            motor_message_to_send.m_start = true;


                            setLED(8, motor_message_to_send.m_up_true);
                            setLED(7, !motor_message_to_send.m_up_true);

                            SendMessageToMotor( '-', motor_message_to_send.m_time_to_spend_in_accel,
                                                    motor_message_to_send.m_time_to_spend_in_cruise,
                                                    motor_message_to_send.m_time_to_spend_in_decel,
                                                    false, true, motor_message_to_send.m_up_true, 0);
                        }

                        // Check to see if the motor is done with the thing we just asked.
                        if (xSemaphoreTake(parameters_for_you->m_service_done, 0))
                        {
                           // So only remove this from the front of the queue
                            // when the motor signals that it is done.
                           
                             motor_message_to_send.m_data = DISTANCE_FROM_GND_TO_P2;
                             motor_message_to_send.state = 'D'; // Update the floor position
                             // motor_message_to_send.m_data holds the distance between floors.
                             motor_message_to_send.m_start = false;
                             //
                            SendMessageToMotor( 'D', 0, 0, 0, false, false,
                                    false, DISTANCE_FROM_GND_TO_P2);

                            xQueueReceive( service_queue_var,
                                                        &temp_req,
                                                        0); // throw temp_req away.
                            UartMessageOut(floor_p2_message);
                            current_floor = 2; // we are on the ground floor now.
                            
                           new_service = true;
                           
                           // Round the motors distance to the floor up.
                          
                           
       
                        }
                        break;

                    case CallToP1InsideCar:
                        if (new_service == true)
                        {

                            new_service = false; // only send the message once.
                                            // we will then wait and check mail
                                           // until the door signals its done.
                            motor_message_to_send = CreateNewMotorMessage(current_max_speed,
                                                                            current_acel,
                                                                            current_floor,
                                                                                      1);

                            motor_message_to_send.m_emer_flag = false;
                            motor_message_to_send.m_start = true;


                            setLED(8, motor_message_to_send.m_up_true);
                            setLED(7, !motor_message_to_send.m_up_true);

                            SendMessageToMotor( '-', motor_message_to_send.m_time_to_spend_in_accel,
                                                    motor_message_to_send.m_time_to_spend_in_cruise,
                                                    motor_message_to_send.m_time_to_spend_in_decel,
                                                    false, true, motor_message_to_send.m_up_true, 0);
                        }

                        // Check to see if the door is done with the thing we just asked.
                        if (xSemaphoreTake(parameters_for_you->m_service_done, 0))
                        {
                           // So only remove this from the front of the queue
                            // when the motor signals that it is done.


                             SendMessageToMotor( 'D', 0, 0, 0, false, false,
                                    false, DISTANCE_FROM_GND_TO_P1);

                            xQueueReceive( service_queue_var,
                                                        &temp_req,
                                                        0); // throw temp_req away.
                            UartMessageOut(floor_p1_message);
                            current_floor = 1; // we are on the ground floor now.
                            new_service = true;
                        }
                        break;
                    case CallToGNDInsideCar:
                        if (new_service == true)
                        {

                            new_service = false; // only send the message once.
                                            // we will then wait and check mail
                                           // until the door signals its done.
                            motor_message_to_send = CreateNewMotorMessage(current_max_speed,
                                                                            current_acel,
                                                                            current_floor,
                                                                                        0);

                            motor_message_to_send.m_emer_flag = false;
                            motor_message_to_send.m_start = true;

                            
                            setLED(8, motor_message_to_send.m_up_true);
                            setLED(7, !motor_message_to_send.m_up_true);

                            SendMessageToMotor( '-', motor_message_to_send.m_time_to_spend_in_accel,
                                                    motor_message_to_send.m_time_to_spend_in_cruise,
                                                    motor_message_to_send.m_time_to_spend_in_decel,
                                                    false, true, motor_message_to_send.m_up_true, 0);
                        }

                        // Check to see if the door is done with the thing we just asked.
                        if (xSemaphoreTake(parameters_for_you->m_service_done, 0))
                        {
                           // So only remove this from the front of the queue
                            // when the motor signals that it is done.

                            SendMessageToMotor( 'D', 0, 0, 0, false, false, false, 0);
                            xQueueReceive( service_queue_var,
                                                        &temp_req,
                                                        0); // throw temp_req away.
                            UartMessageOut(floor_gnd_message);
                            current_floor = 0; // we are on the ground floor now.
                            new_service = true;
                        }
                        break;
                    default: 
                        break;



                }
          }
          else // This is an emergancy
          {

          }
      }

  }
}
///////////////////////
// Returns a new motor message With correct speed, aceleration, decleration
// times for the current_floor to requested_floor.
// DOES NOT set the MotorMessage char[] message, emergancy, or start bool.
///////////////////////////////////////////////////////////////////
MotorMessage CreateNewMotorMessage(float current_max_speed,
                                            float current_acel,
                                            int current_floor,
                                            int requested_floor)
{
    float distance_to_top_speed, acel_and_decel_distance, cruise_distance = 0;
    float time_to_spend_in_acel, time_to_spend_in_cruise = 0;
    float temp_for_testing = 0;
    float tot_disatance;
    MotorMessage motor_message_to_return;
    bool up_true = false;
        distance_to_top_speed =  current_max_speed * current_max_speed / (2 * current_acel);
        acel_and_decel_distance = distance_to_top_speed * 2;
        // Get the cruise distance for our current path.
        if (current_floor == requested_floor)
        {
             time_to_spend_in_acel = 0;
             time_to_spend_in_cruise = 0;
        }
        else
        {  // We have garenteed that the curent_floor != requested_floor
            if (current_floor == 0)
            {
                if (requested_floor == 2)
                {

                    cruise_distance = DISTANCE_FROM_GND_TO_P2 - acel_and_decel_distance;
                     motor_message_to_return.m_data = DISTANCE_FROM_GND_TO_P2;
                }
                else
                {
                    cruise_distance = DISTANCE_FROM_GND_TO_P1 - acel_and_decel_distance;
                    motor_message_to_return.m_data = DISTANCE_FROM_GND_TO_P1;
                }

                up_true = true;


                if (cruise_distance > 0)
                {
                    time_to_spend_in_acel = current_max_speed/current_acel;
                    temp_for_testing = sqrt((2*distance_to_top_speed)/current_acel);
                    time_to_spend_in_cruise = cruise_distance/current_max_speed;
                }
                else // Cruise distance is negative, bad.
                { // Cruise distance will hold how much we are overshooting by
                    cruise_distance = (cruise_distance * -1)/2; // abs and /2
                    // take exactly that much off each acel time
                     distance_to_top_speed  = distance_to_top_speed - cruise_distance;
                   // time for more math.
                    // d = (1/2)at^2 // so t = sqrt([2*d]/a)
                    time_to_spend_in_acel = sqrt((2*distance_to_top_speed)/current_acel);
                    time_to_spend_in_cruise = 0;
                }
            }
            else if (current_floor == 1)
            {     ////////////////////In Penthouse to Penthouse Cases, Will we have
                    ///////////////////to nix the cruise distance entirly, or will the
                            //////////// motor nix that for us?
                                //////////Need to do some testing with penthouse-penthouse
                                   ///////cases with various speeds.
                if (requested_floor == 2)
                {
                   up_true = true;
                   cruise_distance = DISTANCE_FROM_P1_TO_P2 - acel_and_decel_distance;
                    motor_message_to_return.m_data = DISTANCE_FROM_P1_TO_P2;
                }
                else
                {
                    up_true = false;
                    motor_message_to_return.m_data = DISTANCE_FROM_GND_TO_P1;
                    cruise_distance = DISTANCE_FROM_GND_TO_P1 - acel_and_decel_distance;
                }

                if (cruise_distance > 0)
                {
                    time_to_spend_in_acel = current_max_speed/current_acel;
                    time_to_spend_in_cruise = cruise_distance/current_max_speed;
                }
                else // Cruise distance is negative, bad.
                { // Cruise distance will hold how much we are overshooting by
                    cruise_distance = (cruise_distance * -1)/2; // abs and /2
                    // take exactly that much off each acel time
                    distance_to_top_speed  = distance_to_top_speed - cruise_distance;
                   // time for more math.
                    // d = (1/2)at^2 // so t = sqrt([2*d]/a)
                    time_to_spend_in_acel = sqrt((2*distance_to_top_speed)/current_acel);
                    time_to_spend_in_cruise = 0;
                }
            }
            else if (current_floor == 2)
            {
                if (requested_floor == 1)
                {
                    cruise_distance = DISTANCE_FROM_P1_TO_P2 - acel_and_decel_distance;
                    motor_message_to_return.m_data = DISTANCE_FROM_P1_TO_P2;
                }
                else
                {
                    motor_message_to_return.m_data = DISTANCE_FROM_GND_TO_P2;
                    cruise_distance = DISTANCE_FROM_GND_TO_P2 - acel_and_decel_distance;
                }
                up_true = false;

                if (cruise_distance > 0)
                {
                    time_to_spend_in_acel = current_max_speed/current_acel;
                    time_to_spend_in_cruise = cruise_distance/current_max_speed;
                }
                else // Cruise distance is negative, bad.
                { // Cruise distance will hold how much we are overshooting by
                    cruise_distance = (cruise_distance * -1)/2; // abs and /2
                    // take exactly that much off each acel distance
                    distance_to_top_speed  = distance_to_top_speed - cruise_distance;
                   // time for more math.
                    // d = (1/2)at^2 // so t = sqrt([2*d]/a)
                    time_to_spend_in_acel = sqrt((2*distance_to_top_speed)/current_acel);
                    time_to_spend_in_cruise = 0;
                }
            }
////////////////////// (-b - sqrt(b * b - 4 * a * c)) / (2 * a)
            // Now that we know the total cruise distance for this case,

        }
        motor_message_to_return.state = 0x00;
        motor_message_to_return.m_time_to_spend_in_accel = time_to_spend_in_acel;
        motor_message_to_return.m_time_to_spend_in_cruise = time_to_spend_in_cruise;
        motor_message_to_return.m_time_to_spend_in_decel = time_to_spend_in_acel;
        motor_message_to_return.m_emer_flag = false;
        motor_message_to_return.m_start = true;
        motor_message_to_return.m_up_true = up_true;
        return motor_message_to_return;
}

bool QueueServiceRequest( service_req request_for_service,
                            float data_to_go_with_request)
{
    ServiceQueueMessage send_this = { request_for_service, data_to_go_with_request};
    xQueueSendToBack (service_request_message_queue,
                            &send_this,  0 );

    return 0;
}