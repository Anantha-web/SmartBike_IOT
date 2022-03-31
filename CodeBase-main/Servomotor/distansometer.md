### Some pseudo code to the implementation of distansometer

- In this implementation if indicator shows  `0`, then the distance is a large number(or danger is `0`) and if indicator shows a large angle then there is more danger, i.e distance is almost zero.


```
Servo servol

void distansoMeter(int distance)
{
    int safeDistance = 10; // Distance is in SI Units
    int angle = (safeDistance - distance)*120/safeDistance;
    servol.write(angle);

    delay(100);
} 

```