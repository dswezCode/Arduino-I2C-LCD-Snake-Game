
# Arduino-I2C-LCD-Snake-Game
A fully playable classic Snake game built for the Arduino Uno,
Implemented with a standard 16x2 I2C LCD screen (the gameplay runs in a 16x4 res), Analog joystick, passive buzzer and a breadboard.


images showcasing the game:

<img src="https://github.com/user-attachments/assets/82430bbb-7609-440e-a9f3-31467b62728d" width="400">

<img src="https://github.com/user-attachments/assets/a2024ef3-5a93-4ca4-9c07-3706d435157a" width="400">

<img src="https://github.com/user-attachments/assets/0bbb51c2-8b3c-4b6a-a43b-4ab1e3e15769" width="400">









a video showcasing the game:




https://github.com/user-attachments/assets/2faedf5a-2517-4604-8523-de598b46798c








🎮 Try to play online: (wokwi sim)


[![Play Live Version in Wokwi](https://img.shields.io/badge/Play%20Live-Wokwi%20Simulator-blue?style=for-the-badge&logo=arduino)](https://wokwi.com/projects/460763041003336705)




 🐍 GAMEPLAY FEATURES:                                                        
  - Persistent high scores and wins powered by the Arduino's internal        
    EEPROM memory, your High Score and Total Wins are saved permanently    
  - Continuous play mechanic, if you win by getting to maximum              
    snakes length the snake resets while keeping your score                 
    allowing you to continue playing (win count grows by 1 each run you won 
    at least once by achieving maximum length, wins dont stack up in        
    a single run)                                                           
  - Dynamic game speed, the longer you play the faster the snake becomes    
    until it reaches max speed, if you win the game and the snake resets    
    the max speed is kept.                                                  
  - custom tones that play on a passive buzzer to enhance                   
    gameplay experience                                                     
                                                                            
🔧 TECHNICAL FEATURES:                                                       
  - Using dynamically generated custom characters during gameplay loop      
    to generate more custom characters bypassing screen ram limit of only   
    8 graphic characters                                                    
  - Using an engine that allows 16x4 res gameplay on a 16x2 screen          
  - Delta-rendering engine (draws new frames only if change is needed)      
  - Persistent high scores and win tracking (saved to Arduino EEPROM)       
  - State-machine audio engine (non-blocking sound effects to not cause lag)
  - Hardware debounce for smooth joystick controls                          
  - True RNG seeding using human reaction time                              
  - using internal arduino clock to avoid using delay(),                    
    in game loop (to prevent lag)                                           
                                                                            
⚠️ CAUTION:                                                                 
  -The program uses and updates arduinos EEPROM memory to keep track        
   of scores and wins, be sure that you dont need/use the specific          
   slots of this memory before running the program, you also can change     
   the eeprom addresses this program is using by changing the               
   "eeAddressWins" and "eeAddressHighScore" variables                       
   (for example eeAddressWins = 4; eeAddressHighScore=6;)                   
   make sure each address is 2 steps away from the preceding one because int
   variables take 2 places of memory

📺 SCREEN ADDRESS: The program defaults to the screen address 0x27
   if your screen address is diffrent change the 0x27 to your screen address
   in the line LiquidCrystal_I2C lcd(0x27, 16, 2);
                                                                            
🕹️ HOW TO PLAY:                                                             
  -wire everything according to the hardware connections specified below    
   flash the program to the Arduino uno board, power the Arduino board 
   and once the main menu loads simply click on the joystick to begin playing


   

   a "How to wire image" made by wokwi.com
   <img width="1058" height="923" alt="wiring" src="https://github.com/user-attachments/assets/43d55f3c-ac85-43fe-b12f-ecd8db1ea891" />





   Image of my setup:
   

   <img src="https://github.com/user-attachments/assets/cc518dce-afee-4b06-b02f-e82c8669c28f" width="400">



   







   
                                                                            
 🔌 HARDWARE CONNECTIONS:                                                     
  - Joystick 5v             -> 5v supply                                        
  - Joystick GND            -> GND                                              
  - Joystick X-Axis         -> Analog A0                                        
  - Joystick Y-Axis         -> Analog A1                                        
  - Joystick Button/SW      -> Digital Pin 8                                    
  - Passive Buzzer (plus)   -> Digital Pin 9                                    
  - Passive Buzzer (minus)  -> GND                                              
  - 16x2 I2C LCD VCC        -> 5v supply                                        
  - 16x2 I2C LCD GND        -> GND                                              
  - 16x2 I2C LCD SDA        ->Arduino SDA pin
  - 16x2 I2C LCD SCL        ->Arduino SCL pin                                   
                                                                            
📖 LIBRARIES REQUIRED:                                                       
  - LiquidCrystal_I2C (by Frank de Brabander/John Rickman)                                                    
  - EEPROM (Built-in)                                                 
