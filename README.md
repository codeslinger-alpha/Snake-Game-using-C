**Implementation of a basic snake game🐍, using pointers and 2D array.**  
# Controls  
Keys are **not case-sensitive**   
`w`=up  
`a`=left  
`s`=down  
`d`=right  
`space`=pause/unpause  
# Features  
**Border wrapping**
### In the *source code*  
```c
#define bgcolor 1
#define ROW 15
#define COL 30
int wait_time_ms=1000;//This is the maximum value
int border_wall;
char prev = 'd'; // primary direction of movement
const char snake_body = '=', point = 'o', OBSTACLE = '#'; // Visuals
```
-change `ROW` and `COL` to change the area  
-set `bgcolor` to 1 or 0 to turn the background colour on or off  
-change `wait_time_ms` to change frame speed  
-change `prev` to change the direction in which the snake starts moving  
-**you can change snake design also**  
### After running the program  
-Select level(speed)  
-You can set boundary walls  
-Set obstacles randomly  
-Beep sound will play if you eat a point or when it's game over🎮  
# Demo  
## Screenshots 
**Boundary**  
![image](https://github.com/user-attachments/assets/887853fe-74a2-47b1-8c2a-3c1d0fb2f68b)  
**Border wrapping**  
![image](https://github.com/user-attachments/assets/63a1ca6c-6770-4968-a0b5-cce786c88f1d)  
## Output    
<p align="left">
  <img src="https://github.com/user-attachments/assets/23b39816-a0f8-4021-9ad1-39890a3f21b6" alt="gameplay">
</p> 





