#define TRIG_PIN 13
#define ECHO_PIN 12
#define SONAR_VCC_PIN 11


/* Measure distance in cm using ultrasonic sensor */
int measureDistanceFromSonar();

/* Get the average distance from ultrasonic sensor after n_measurements, returns in cm*/
int averageDistanceFromSonar(int n_measurements = 5);
