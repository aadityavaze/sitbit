#include <Arduino_APDS9960.h>
#include <Arduino_HTS221.h>
#include <Arduino_LPS22HB.h>
#include <Arduino_LSM9DS1.h>
#include <PDM.h>
#include <climits> 


#define HEART_LIGHT 8   

#define PERSON_SITTING 0
#define PERSON_EXERCISING 1
#define PERSON_IDLE 2
#define REMIND_PERSON_TO_STAND 3
#define PERSON_CHEATING 4
#define IDEAL_NO_OF_UPDATES_TO_SIT 180
#define IDEAL_NO_OF_UPDATES_TO_EXERCISE 180


int userState = PERSON_IDLE;


char buff[100];
int chairClock = 0;
float heartIntensity = -1.0;
int updateIntensity = 1;
int updateRate1 = 0;
int i = 0;
long lastUpdate1;
int totalUpdates1;
int updateRate2 = 100;
long lastUpdate2;
int totalUpdates2;

int rotationVal = 0;


void setup() {
    pinMode(HEART_LIGHT, OUTPUT);
    
    Serial.begin(115200);
    
    if (!APDS.begin()) {
        Serial.print("APDS init failed");
        while (1) {}
    }
}

void loop() {


  switch(userState){
    case PERSON_IDLE: 
    updateRate2 = INT_MAX;
    updateRate1 = INT_MAX;
   analogWrite(8, 255);
    break;
    case PERSON_SITTING: updateRate2 = 50;
    updateRate1 = 50;
    
    break;
    case PERSON_EXERCISING: updateRate2 = 50;
    updateRate1 = 50;
    break;

    case PERSON_CHEATING: updateRate2 = 1;
    updateRate1 = INT_MAX;
    break;

    case REMIND_PERSON_TO_STAND: updateRate2 = 1;
    updateRate1 = INT_MAX;
    break;
    }

    if(userState == PERSON_SITTING){
       if(totalUpdates1> IDEAL_NO_OF_UPDATES_TO_SIT){
                userState = REMIND_PERSON_TO_STAND;
                totalUpdates1 = 0;
                }
          }
     if(userState == PERSON_EXERCISING){
        if(totalUpdates1> IDEAL_NO_OF_UPDATES_TO_EXERCISE){
                userState = PERSON_IDLE;
                totalUpdates1 = 0;
                }
        }

    bool proximityAvailable;
    int proximity = -1;
    
    if ((proximityAvailable = APDS.proximityAvailable())) {
    proximity = APDS.readProximity();
    //Serial.println(proximity);
    }

    
     if (proximityAvailable) {

       if(proximity <= 220 && userState == PERSON_IDLE){
           userState = PERSON_SITTING;
          }
          

       if(proximity > 220 && userState == PERSON_SITTING){
          userState = PERSON_IDLE;
        }
      

       if(proximity > 220 && userState == REMIND_PERSON_TO_STAND){
           userState = PERSON_EXERCISING;
           totalUpdates1 = 0;
          }

        if(proximity > 220 && userState == PERSON_CHEATING){
           userState = PERSON_EXERCISING;
                  }

          if(millis() - lastUpdate1>=updateRate1)
        {
            lastUpdate1 = millis();
            Serial.println(userState);
            
               if(proximity > 220){
                if(userState == PERSON_SITTING){
                    userState = PERSON_IDLE;
                    totalUpdates1 = 0;
     
                  }
               
                if(userState == PERSON_EXERCISING){
                  totalUpdates1++;
                  }
               
                        
              }
            else{
              if(userState == PERSON_SITTING){
                 totalUpdates1++;
                }
              if(userState == PERSON_EXERCISING){
                totalUpdates1 = 0;
                userState = PERSON_CHEATING;
                
                }
              else if(userState != PERSON_SITTING){
                userState = PERSON_SITTING;
                }
               
              }
         
        }

       
      }


      if(millis() - lastUpdate2>=updateRate2){
        

        lastUpdate2 = millis();
        analogWrite(8,map(100*(1.0 + sin(heartIntensity)),0,200,0,255));
        heartIntensity = heartIntensity + 0.05;

    
        }

}
