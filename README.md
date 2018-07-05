# nrf_receiver_profiling
/* The purpose of this module is to toggle pins on the nRF51 at certain events so that we can measure
how the receiver spends its time with a logical analyzer.

QUICK START:
Enable this module by calling "receiver_profiling_init() at the end of the initializion of the RADIO in main


REQUIREMENTS: 
* This .c file and it's .h file must lie within the project and the nrf.h must be on 
the include path.
* Must have at least 1 available GPIOTE TASK/EVENT Register. We will use OUT[3] as default
* At least 3 available PPI-channels. We will use CH13, CH14, CH15 by default
* At least 1 available GPIO PIN. Default is PIN30


ASSUMPTIONS ABOUT YOUR SYSTEM:
We have made the following assumptions about the design of your radio:
* Shortcut END->DISABLED enabled
* Shortcut RXEN->START enabled
* RX_EN is called during init. Thus the Receiver is in RX state when the test start. If the Receiver is in any other state the output on the PIN will not be decoded correctly be the python script (the first pin toggle corresponds to an ADDRESS event)
* The flow of tasks/events of the receiver is assumed to be:


RX(state) -> ADDRESS(event) -> PAYLOAD(event) -> END(event) -> (shortcut) -> DISABLED. # PROCESSING OF PACKAGE # -> RADIO_RXEN(task)

I.e. that Receiver starts in the RX state and calls RXEN once he is finished with receiving and processing a packet.

HOW-IT-WORKS
We generate PIN-toggles on PIN30 at the following events: READY, PAYLOAD, END. We assume that the system is in state RX
when we start the test. The output on PIN30 should be as following for when receiving ONE single packet:


                    ADDRESS                   PAYLOAD     END                 READY      
                      |_________________________|          |___________________|
                      |                         |          |                   |
                      |                         |          |                   |
          ____________|                         |__________|                   |________________

          
                      |       RECEIVE_TIME      | OVERHEAD |  PACKET PROCESS   |
                      |       (- Access Address | (CRC etc)|   (+ RAMP UP)     |
                      |           and preamble) |          |                   |

We will provide a Python3 script that will automate the reading and caluclation with a SALAE LOGIC ANALYZER

COMMON PITFALLS:
* GPIOTE[4], PPI[13], PPI[14], PPI[15] or PIN30 is already in use. 
SOLUTION: Change the relevant values in this .c file
* configuration of PPI or GPIOTE elsewhere in your project is overwriting the configuration made in this 
SOLUTION: Use the '|=' rather than the '=' operator to configurate the registers in your code elsewhere.
* The output doesnt match the one drawn above
    POSSIBLE EXPLANATION: The radio is not in RX when the test begin. Maybe it is already listening to a packet or hasnt been initialized already.

*/
