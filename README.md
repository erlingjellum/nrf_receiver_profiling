# nrf_receiver_profiling
/* The purpose of this module is to toggle GPIO pins on the nRF51/nRF52 at certain events so that we can measure and profile the receiver. 

QUICK START:
1. Copy the .c file and the .h file into your nRF DK project directory
2. Enable this module by calling including receiver_profiling.h and running "receiver_profiling_init() at the end of the initializion of the RADIO in the main() of your project.
3. Connect a logical analyzer to PIN30
4. Run tests and watch the output on the UI of the Logical Analyzer

REQUIREMENTS: 
* This .c file and it's .h file must lie within the project. If using KEIL uVision you must also add both files to the your project
* Include "receiver_profiling.h" in main.c
the include path.
* Must have at least 1 available GPIOTE TASK/EVENT Register. We will use OUT[3] by default
* At least 4 available PPI-channels. We will use CH12 CH13, CH14, CH15 by default
* At least 1 available GPIO PIN. Default is PIN30


ASSUMPTIONS ABOUT YOUR SYSTEM:
We have made the following assumptions about the design of your radio:
* Shortcut END->DISABLED enabled
* Shortcut RXEN->START enabled
* RX_EN is called during init. Thus the Receiver is in RX state when the test start. If the Receiver is in any other state the output on the PIN will not be decoded correctly be the python script (the first pin toggle corresponds to an ADDRESS event)
* The flow of tasks/events of the receiver is assumed to be:


RX(state) -> ADDRESS(event) -> PAYLOAD(event) -> END(event) -> (shortcut) -> DISABLED (state). # PROCESSING OF PACKAGE # -> RADIO_RXEN(task)->RADIO_READY(event) -> (shortcut) -> RX(state)

(Events give rise to pin toggles. Tasks are executed by your program. States are states of your Radio) 

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

POSSIBLE ERRORS:
* GPIOTE[4], PPI[13], PPI[14], PPI[15] or PIN30 is already in use. 
SOLUTION: Change the relevant values in this .c file
* configuration of PPI or GPIOTE elsewhere in your project is overwriting the configuration made in this 
SOLUTION: Use the '|=' rather than the '=' operator to configurate the registers in your code elsewhere.
* The output doesnt match the one drawn above
    POSSIBLE EXPLANATION: The radio is not in RX when the test begin. Maybe it is already listening to a packet or hasnt been initialized already.

*/
