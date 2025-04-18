#include <Encoder.h>
#include <Servo.h>

Encoder knobLeft(2, 7);
Encoder knobRight(3, 8);

Servo servoR, servoL;

int LED_White = 5;
int LED_Red = 4;

int Rdown = 110;
int Rup = 160;
int Ldown = 75;
int Lup = 25;

int current_act = 0;
int act = 0;
int i = 0;
int DT = 14;
int Q_Previous_action = 5;
float d_ant = 0;
int actarray[8] = {0, 1, 2, 3, 0, 1, 2, 3};
unsigned long oldtime = 0;
unsigned long newtime = 0;
unsigned long Tdiff = 0;
long newposition = 0;
long oldposition = 0;
long Pdiff = 0;
long  old_time = 0;
long  new_time = 0;
int len = 4;
int wid = 4;

void setup() {
  Serial.begin(9600);
  //Setting Up Pins
  pinMode(LED_Red, OUTPUT);
  pinMode(LED_White, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);

  servoR.attach(10);
  servoL.attach(11);
  
  Serial.println("");
  Serial.println("Q-Bert Is Ready to Learn!!");
  Serial.println("----------------------------------------");
   
  for (i = Rdown; i <= Rup; i += 1) {
    servoR.write(Rup);
    servoL.write(Lup);
    digitalWrite(LED_Red, HIGH);
    digitalWrite(LED_White, HIGH);
    delay(100);
  }
  int iter2 = 0;
  int iter = 0;

  /////Qtable
  float q_matrix[4][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  };

  float rewards[4][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
  };

  float actions[4][4] = {
    {0, 1, 2, 3},
    {0, 1, 2, 3},
    {0, 1, 2, 3},
    {0, 1, 2, 3}
  };

  ////Q_Learning Process

  Serial.println("\nQ Matrix");
  for (iter = 0; iter < len; iter += 1) {
    for (iter2 = 0; iter2 < wid; iter2 += 1) {
      Serial.print("  ");
      Serial.print(q_matrix[iter][iter2]);
      delay(1);
    }
    Serial.println();
  }

  digitalWrite(LED_Red, HIGH);
  digitalWrite(LED_White, LOW);


  //////////////////////////////////Outer Loop
  int learninglength = 100;
  int iter3 = 0;
  for (iter3 = 0; iter3 <= learninglength; iter3 += 1) {

    ////////////Move Robot
    int iterL = 0;
    int iterW = 0;
    for (iterL = 0; iterL <= 3; iterL += 1) {
      int randomaction = random(4);
      int act = actions[iterL][randomaction];
      if (act == 0) {
        for (i = Rdown; i <= Rup; i += 1)//UP Both
        {
          servoR.write(Rup);//Rup
          servoL.write(Lup);//Lup
          delay(DT);
        }

      }
      else if (act == 1) {
        for (i = Rdown; i <= Rup; i += 1)
        {
          servoR.write(Rdown);//Rdown
          servoL.write(Ldown);//Ldown
          delay(DT);
        }
      }
      else if (act == 2) {
        for (i = Rdown; i <= Rup; i += 1)
        {
          servoR.write(Rup);//Rup
          servoL.write(Ldown);//Ldown
          delay(DT);
        }
      }
      else if (act == 3) {
        for (i = Rdown; i <= Rup; i += 1)
        {
          servoR.write(Rdown);//Rdown
          servoL.write(Lup);//Lup
          delay(DT);
        }
      }
      /////////////////////////////////////////////////////////

      /////////////////////////////////////////////////////////////////////////////////////////////////Bad Habit
      int Q_current_action = act;

      if (Q_current_action == Q_Previous_action) {
        ////Add large negative reward of -1
        int current_act = act;
        ///////////////////////////////////////Add negative reward
        /////////////////////////////////////////////Update Table based on reward from movement
        int current_state = iterL;
        int action = randomaction;
        //Serial.println("Updating Table check");
        float gamma = 0.9;
        ///////////////////////////////Major Change
        float sa_reward = -1000;
        /////////////////////////////////Major Change
        float memory = q_matrix[current_state][action];
        float d_new = dist();
        float diff = d_new - d_ant;
        d_ant = d_new;
        float ns_reward = diff;
        float q_current_state = sa_reward + (gamma * ns_reward) + memory;
        q_matrix[current_state][action] = q_current_state;
        float new_state = action;
        Q_Previous_action = act;

      }
      ////////////////////////////

      ///////////////////////////////////////////////////////////////////////////////////Punished if bad


      ///////////////////////////////////////////////////////////Reward Normal
      else {
        /////////////////////////////////////////////Update Table based on reward from movement
        int current_state = iterL;
        int action = randomaction;
        //Serial.println("Updating Table check");
        float gamma = 0.9;
        float sa_reward = rewards[current_state][action];
        float memory = q_matrix[current_state][action];
        float d_new = velocity();
        float diff = d_new - d_ant;
        d_ant = d_new;
        float ns_reward = diff;
        float q_current_state = sa_reward + (gamma * ns_reward) + memory;
        q_matrix[current_state][action] = q_current_state;
        float new_state = action;
        Q_Previous_action = act;
      }
    }
  }

  ///Display Q and Reward Matrix
  Serial.println("\nQ Matrix");
  for (iter = 0; iter < len; iter += 1) {
    for (iter2 = 0; iter2 < wid; iter2 += 1) {
      Serial.print("  ");
      Serial.print(q_matrix[iter][iter2]);
      delay(1);
    }
    Serial.println();
  }

  digitalWrite(LED_Red, LOW);
  digitalWrite(LED_White, HIGH);

  int len = 4;
  int previous_act = 0;
  int iter5 = 0;
  int iter5_2 = 0;
  float Reward_Large_enough = 2000;

  int colIndex[4] = {0, 0, 0, 0};

  for (byte i = 0; i < 4; i++)
  {
    int maxValue = -1;//q_matrix[0][0];
    for (byte j = 0; j < 4; j++) {
      if (q_matrix[i][j] > maxValue) {
        maxValue = q_matrix[i][j];
        colIndex[i] = j;
      }
      maxValue =  -1; ////////////Found it
    }
  }

  if (colIndex[0] == colIndex[1] && colIndex[1] == colIndex[2] && colIndex[2] == colIndex[3])
    {colIndex[0]=0;}
  Serial.println("");
  Serial.println("Action Index");
  Serial.println(colIndex[0]);
  Serial.println(colIndex[1]);
  Serial.println(colIndex[2]);
  Serial.println(colIndex[3]);
  while (1) {

    for (int iter = 0; iter < 4; iter++) {
      
      
      act  = colIndex[iter];



      if (act == 0) {
        for (i = Rdown; i <= Rup; i += 1)//UP Both
        {
          servoR.write(Rup);//Rup
          servoL.write(Lup);//Lup
          delay(DT);
        }

      }
      else if (act == 1) {
        for (i = Rdown; i <= Rup; i += 1)
        {
          servoR.write(Rdown);//Rdown
          servoL.write(Ldown);//Ldown
          delay(DT);
        }
      }
      else if (act == 2) {
        for (i = Rdown; i <= Rup; i += 1)
        {
          servoR.write(Rup);//Rup
          servoL.write(Ldown);//Ldown
          delay(DT);
        }
      }
      else if (act == 3) {
        for (i = Rdown; i <= Rup; i += 1)
        {
          servoR.write(Rdown);//Rdown
          servoL.write(Lup);//Lup
          delay(DT);
        }
      }
    }
  }
}

void loop() {
} // end loop

float dist() {  // routine to measure distance

  long Right_distance = knobLeft.read();
  //long Left_distance = knobLeft.read();
  long distance = Right_distance;
  //-Left_distance;
  return distance;
}

float velocity() {  // routine to measure velocity

  newtime = millis();
  newposition = knobRight.read();
  Tdiff = newtime - oldtime;
  Pdiff = newposition - oldposition;
  oldtime = newtime;
  oldposition = newposition;
  long velocity = Pdiff * (360.0 / 2400.0 / Tdiff) * 1000;
  return velocity;

}
