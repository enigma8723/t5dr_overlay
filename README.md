The project is work in progress.

It is a console application that reads out the game's memory contents. You can use it to display the frame data for p1's moves.

![Alt text](screenshot.png)

Only NPEA00019 is supported.

The console application does not modify the game in any way. Only its memory is read.
The source code contains no copyrighted data.

How do you use the console application?

1. You start the game.
2. You go to practice mode and chose characters for p1 and p2 as well as the stage.
3. You wait to be in-game.
4. You launch the console application.
5. Now when a p1 move connects with p2 the following information is printed to the console application:
- p1 move id
- p1 active frames
- p1 frame (dis)advantage (based on the first active frame)
- p1 anim length (sum of startup, active and recovery frames)
- if the p1's move connected
- p2 move id
- p2 frame (dis)advantage (based on the first active frame)
- p2 anim length (sum of startup, active and recovery frames)
- if the p2's move connected (always 0 for now)
6. The output remains in the console application until p1 connects another move with p2.

Planned improvements:
- display of frame data when a p2 move connects with p1
- frame (dis)advantage based on which active frame the move connected on (if I can find the memory address at which this is being kept track of)
- a proper overlay instead of a console application
- improving performance of the memory readout (currently when you do a string and input the command for it fast, e.g. 1,2, the frame data from 1 might not be printed out because the logic doesn't manage to read out 1 in time before 2 becomes relevant)


Acknowledgements:
- A big thank you to Kiloutre who created the Tekken Moveset Extractor and provided its source code here on github (https://github.com/Kiloutre/TKMovesets). To make this console application I analyzed the relevant parts of his tool and used what I learned to write my own condensed code as well as used some small part of his logic, types, classes and helper functions.
  Without his work I couldn't have done it!
- A big thank you also to T-vk who created a short, easy to use class to attach to processes and read out their memory (https://github.com/T-vK/Memory-Hacking-Class). His class is used in the console application to do just that.
- Thank you talso to RASKALOF ОНЛИФАНСУ from Discord who gave me the idea to implement my own overlay (he has made one before me; more pretty too :)).
