#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <assert.h>
#include "portable.h"
#include "stewie-lamps.h"
#include "lamps.h"
#include "sounds.h"
#include "game.h"
#include "display.h"

#include "switches.h"


static char* switchName(int gpio) {
   switch( gpio ) {
       case SWITCH_START:         return "start";
       case SWITCH_FLIPPER_LEFT:  return "flipper_left";
       case SWITCH_FLIPPER_RIGHT: return "flipper_right";
       case SWITCH_LOIS:          return "louis";
       case SWITCH_MEG:           return "meg";
       case SWITCH_PETER:         return "peter";
       case SWITCH_SHOOTER:       return "shooter";
       case SWITCH_BRIAN_CHRIS:   return "brian_chris";
       case SWITCH_CHRIS_BRIAN:   return "chris_brian";
       default: return "?"; 
   }
}

static void shooterOff(void) {
   gpioWrite( SOLENOID_SHOOTER, 0 );
   gpioCancelTimer( TIMER_SHOOTER );
}

static void switchCallback(int gpio, int level, uint32_t tick) {
    static bool brian_sw = false;
    static bool chris_sw = false;
    static int balls = 0;
    
    printf("switch %s %d\n", switchName(gpio), level);
    
    if ( balls <= 0 ) {
        if ( gpio == SWITCH_START ) {
            if ( level == 0 ) {
                balls = 5;
                printf("balls:%d\n", balls);
                soundPlay( sound_start );
                gameStart();
            }
        }
    }
    
    switch ( gpio ) {
            
       case SWITCH_START:
           if ( level == 0 ) {
              if ( gpioRead( SWITCH_SHOOTER ) == 0 ) {
                 soundPlay( sound_launch );
                 gpioWrite( SOLENOID_SHOOTER, 1 );
                 gpioSetTimerFunc( TIMER_SHOOTER, 100, shooterOff );
              }
           }
           break;
            
        case SWITCH_SHOOTER:
            if ( level == 0 ) {
                balls--;
                printf("balls:%d\n", balls);
                if ( balls > 0 ) {
                    lampsBlink( blink_shoot_again );
                    soundPlay( sound_drain );
                    displayBall( balls );
                } else {
                    lampsBlink( blink_none );
                    gameStop();
                    soundPlay( sound_game_over );
                    displayText( "Damn" );
                }
            } else {
                lampsBlink( blink_none );              
            }
            break;
            
        case SWITCH_LOIS: {
            if ( level != 0 ) {
               break;
            }
            static int j = 0;
            j |= 0x01;
            ledMatrixLois( j );
            j <<= 1;
            if ( j > 0x0F ) {
                j = 0;
                soundPlay( sound_lois );
                lampsBlink( blink_lois );
                displayScore( 100 );
            } else {
            	displayScore( 10 );
            }
            soundPlay( sound_hit );
            break; }   

        case SWITCH_MEG: {
            if ( level != 0 ) {
               break;
            }
            static int j = 0;
            j |= 0x01;
            ledMatrixMeg( j );
            j <<= 1;
            if ( j > 0x07 ) {
                j = 0;
                soundPlay( sound_meg );
                lampsBlink( blink_meg );
                displayScore( 100 );
            } else {
                displayScore( 10 );
	    }
            soundPlay( sound_hit );
            break; } 

        case SWITCH_PETER: {
            if ( level != 0 ) {
               break;
            }
            static int j = 0;
            j |= 0x01;
            ledMatrixPeter( j );
            j <<= 1;
            if ( j > 0x1F ) {
                j = 0;
                soundPlay( sound_peter );
                lampsBlink( blink_peter );
                displayScore( 100 );
            } else {
                displayScore( 10 );
            }
            soundPlay( sound_hit );
            break; } 

        case SWITCH_BRIAN_CHRIS: {
            if ( level != 0 ) {
               break;
            }
            if ( !brian_sw ) {
               chris_sw = true;
               break;
            }
            brian_sw = false; 
            static int j = 0;
            j |= 0x01;
            ledMatrixChris( j );
            j <<= 1;
            if ( j > 0x1F ) {
                j = 0;
                soundPlay( sound_chris );
                lampsBlink( blink_chris );
                displayScore( 100 );
            } else {
                displayScore( 10 );
            }
            soundPlay( sound_hit );
            break; } 

        case SWITCH_CHRIS_BRIAN: {
            if ( level != 0 ) {
               break;
            }
            if ( !chris_sw ) {
               brian_sw = true;
               break;
            }
            chris_sw = false;
            static int j = 0;
            j |= 0x01;
            ledMatrixBrian( j );
            j <<= 1;
            if ( j > 0x1F ) {
                j = 0;
                soundPlay( sound_brian );
                lampsBlink( blink_brian );
                displayScore( 100 );
            } else {
                displayScore( 10 );
            }
            soundPlay( sound_hit );
            break; } 
    }
}
    
void switchesInit(void) {
    printf("switchesInit()\n");
    int switches[] = {
        SWITCH_START,
        SWITCH_LOIS,
        SWITCH_MEG,
        SWITCH_PETER,
        SWITCH_SHOOTER,
        SWITCH_BRIAN_CHRIS,
        SWITCH_CHRIS_BRIAN
    };
    for (int i=0; i<sizeof(switches)/sizeof(switches[0]); i++) {
        int gpio = switches[i];
        gpioSetMode( gpio, PI_INPUT );
        gpioSetPullUpDown( gpio, PI_PUD_UP );
        gpioGlitchFilter( gpio, 250 );
        gpioSetAlertFunc( gpio, switchCallback );
    }
    gpioGlitchFilter( SWITCH_SHOOTER, 2000 );
    gpioSetMode( SWITCH_DRIVE_SIGNAL, PI_OUTPUT );
    gpioWrite( SWITCH_DRIVE_SIGNAL, 0 );
}


