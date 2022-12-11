//#include <dummy.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

//variables for A*
#include <stdio.h>
#include <stdlib.h>

#define  SIZE   25      /* number of cities */

int      START = 0;         /* index of start city, set interactively */
#define  GOAL   44      /* index of goal city, Vienna */

#define  CLOSED  0      /* flags for open and closed lists */
#define  OPEN    1

#define  TRUE    1
#define  FALSE   0

int lightArr[8];
int lightIndex = 7;

int goalRow = GOAL / 5;
int goalColumn = GOAL % 5;

//String num; 
// int number;

typedef  struct city {
         char           name[2];
         int            ind;            /* position in array cities */
         int            g_val;          /* optimal path to city */
         int            h_est;          /* estimate to goal */
         int            f_est;          /* g_value + h_estimate */
         int            open;           /* open list */
         int            closed;         /* closed list */
         struct city    *adjcs[ SIZE ]; /* adjacent cities */
         struct city    *backptr;       /* optimal path pointer */
} CITY;

CITY  cities[ SIZE ];

int   distances[ SIZE ][ SIZE ];   /* distances between two cities */
int   open_count = 0;              /* number of items on open list */

void  get_input( void ),
      set_adjacency_lists( void ),
      astar( void );
void  failure( void ), success( const CITY *ptr ),
      expand( int index );
int   get_best_prospect( void );
void  update_values( CITY *ptr ),
      update_path( CITY *ptr, int index );
void  get_start_city( void );
void  print_cities( const CITY *ptr );

//---------------------------------------------

//variables for WiFi.
const char* ssid = "Government";
const char* password = "123456789";
const char* servURL = "http://192.168.4.1/";
String received = "";
//---------------------------------------------

void setup() {
  // put your setup code here, to run once:

  //for WiFi.
  Serial.begin(9600);

  for(int i = 0; i < 8; i++)
  {
    lightArr[i] = -1;
  }
  pinMode(D0, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D7, OUTPUT);
  pinMode(D8, OUTPUT);

  //digitalWrite(1, HIGH);

  //for A*(building the adjacency list).
  cities[ 0 ].adjcs[ 0 ] = &cities[ 1 ];

        /*********************************************************/
        cities[ 1 ].adjcs[ 0 ] = &cities[ 0 ];
        cities[ 1 ].adjcs[ 1 ] = &cities[ 2 ];
        cities[ 1 ].adjcs[ 2 ] = &cities[ 6 ];

        /*********************************************************/
        cities[ 2 ].adjcs[ 0 ] = &cities[ 1 ];

        /*********************************************************/
        cities[ 3 ].adjcs[ 0 ] = &cities[ 8 ];

        /*********************************************************/
        cities[ 4 ].adjcs[ 0 ] = &cities[ 9 ];

        /*********************************************************/
        cities[ 5 ].adjcs[ 0 ] = &cities[ 6 ];

        /*********************************************************/
        cities[ 6 ].adjcs[ 0 ] = &cities[ 1 ];
        cities[ 6 ].adjcs[ 1 ] = &cities[ 5 ];
        cities[ 6 ].adjcs[ 2 ] = &cities[ 7 ];
        cities[ 6 ].adjcs[ 3 ] = &cities[ 11 ];

        /*********************************************************/
        cities[ 7 ].adjcs[ 0 ] = &cities[ 6 ];
        cities[ 7 ].adjcs[ 1 ] = &cities[ 8 ];

        /*********************************************************/
        cities[ 8 ].adjcs[ 0 ] = &cities[ 3 ];
        cities[ 8 ].adjcs[ 1 ] = &cities[ 7 ];
        cities[ 8 ].adjcs[ 2 ] = &cities[ 9 ];
        cities[ 8 ].adjcs[ 3 ] = &cities[ 13 ];

        /*********************************************************/
        cities[ 9 ].adjcs[ 0 ] = &cities[ 4 ];
        cities[ 9 ].adjcs[ 1 ] = &cities[ 8 ];

        /*********************************************************/
        cities[ 10 ].adjcs[ 0 ] = &cities[ 15 ];

        /*********************************************************/
        cities[ 11 ].adjcs[ 0 ] = &cities[ 6 ];
        cities[ 11 ].adjcs[ 1 ] = &cities[ 16 ];

        /*********************************************************/
        cities[ 12 ].adjcs[ 0 ] = &cities[ 13 ];  /* Rome */

        /*********************************************************/
        cities[ 13 ].adjcs[ 0 ] = &cities[ 8 ];
        cities[ 13 ].adjcs[ 1 ] = &cities[ 12 ];
        cities[ 13 ].adjcs[ 2 ] = &cities[ 14 ];
        cities[ 13 ].adjcs[ 3 ] = &cities[ 18 ];

        /*********************************************************/
        cities[ 14 ].adjcs[ 0 ] = &cities[ 13 ];

        /*********************************************************/
        cities[ 15 ].adjcs[ 0 ] = &cities[ 10 ];
        cities[ 15 ].adjcs[ 1 ] = &cities[ 16 ];
        cities[ 15 ].adjcs[ 2 ] = &cities[ 20 ];
        /*********************************************************/
        cities[ 16 ].adjcs[ 0 ] = &cities[ 11 ];
        cities[ 16 ].adjcs[ 1 ] = &cities[ 15 ];
        cities[ 16 ].adjcs[ 2 ] = &cities[ 17 ];
        cities[ 16 ].adjcs[ 3 ] = &cities[ 21 ];

        /********************************************************/
        cities[ 17 ].adjcs[ 0 ] = &cities[ 16 ]; 
        cities[ 17 ].adjcs[ 1 ] = &cities[ 18 ]; 

        /*********************************************************/
        cities[ 18 ].adjcs[ 0 ] = &cities[ 13 ];
        cities[ 18 ].adjcs[ 1 ] = &cities[ 17 ];
        cities[ 18 ].adjcs[ 2 ] = &cities[ 19 ];
        cities[ 18 ].adjcs[ 3 ] = &cities[ 23 ];

        /*********************************************************/
        cities[ 19 ].adjcs[ 0 ] = &cities[ 18 ];

        /*********************************************************/
        cities[ 20 ].adjcs[ 0 ] = &cities[ 15 ];

        /*********************************************************/
        cities[ 21 ].adjcs[ 0 ] = &cities[ 16 ];

        /*********************************************************/
        cities[ 23 ].adjcs[ 0 ] = &cities[ 18 ];
        cities[ 23 ].adjcs[ 1 ] = &cities[ 24 ];

        /*********************************************************/
        cities[ 24 ].adjcs[ 0 ] = &cities[ 23 ];

//////////////////////////////////////////////////////////////////////////

  /*for WiFi*/
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  WiFi.begin(ssid, password);
  Serial.println("Connecting");

  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println();
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  //---------------------------------------------

}

void loop() {
  // put your main code here, to run repeatedly:

  if(WiFi.status()== WL_CONNECTED )
  { 
    //call httpGetRequest handler.
    received = httpGETRequest(servURL);
    
    //get Start and End nodes and call aStar().
    if(received[1] != '8')
    {
      START = received.toInt();
      get_input();
        //set_adjacency_lists();

        cities[ START ].backptr = NULL; /* path starts here */
        cities[ START ].g_val = 0;

        cities[ START ].open = TRUE;    /* put START on open list */
        ++open_count;

        astar();  /* search */
    }
  }

}

//httpGetRequest handler.
String httpGETRequest(const char* serverName)
{
  WiFiClient client;
  HTTPClient http;

  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "--"; 
  
  if (httpResponseCode > 0)
  {
    payload = http.getString();
  }

  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  //Free resources
  http.end();

  return payload;
}

void astar()
{
        int best;

        if ( open_count == 0 )            /* any more options? */
             failure();                   /* if not, fail */
        else {
             best = get_best_prospect();      /* get most promising city */
             if ( best == GOAL )              /* goal city? */
                  success( &cities[ GOAL ] ); /* if so, succeed */
             else {                           /* otherwise: */
                  cities[ best ].open = FALSE;    /* remove from open */
                  --open_count;
                  cities[ best ].closed = TRUE;   /* put on closed */
                  expand( best );                 /* expand it */
                  astar();                        /* keep searching */
             }
        }
}

int get_best_prospect()
{
        int  i = 0, best;

        /* Find 1st city in open list. */
        while ( cities[ i++ ].open == FALSE )
                        ;
        /* Assume it's the best. */
        best = --i;

        /* Look for something better. */
        for ( i = best + 1; i < SIZE; ++i )
            if ( cities[ i ].open == TRUE  &&
                 cities[ i ].f_est  <  cities[ best ].f_est )
               best = i;

        return best;
}

void expand( int index )
{
        int i = 0;

        while ( cities[ index ].adjcs[ i ] != NULL ) {

             /* If adjacent city is not on OPEN or CLOSED, then      */
             /* (1) set its backpointer to current city, (2) compute */
             /* its g_val and f_est, and (3) put it on OPEN.         */
             if ( cities[ index ].adjcs[ i ] -> open == FALSE  &&
                  cities[ index ].adjcs[ i ] -> closed == FALSE ) {
                cities[ index ].adjcs[ i ] -> backptr = &cities[ index ];
                update_values( cities[ index ].adjcs[ i ] );
                cities[ index ].adjcs[ i ] -> open = TRUE;
                ++open_count;
             }
             /* Otherwise, see if its backpointer needs changing. */
             else
                update_path( cities[ index ].adjcs[ i ], index );
             ++i;
        }
}

void  update_values( CITY *ptr )
{
        int ind1, ind2;

        ind1 = ptr -> backptr -> ind;   /* current city */
        ind2 = ptr -> ind;              /* predecessor */
        ptr -> g_val = ptr -> backptr -> g_val  +  distances[ ind1 ][ ind2 ];
        ptr -> f_est = ptr -> g_val  +  ptr -> h_est;
}

void  update_path( CITY *ptr, int index )
{
        CITY  *best;

        /* Assume current path is best. */
        best = ptr -> backptr;

        /* Compare with new path. */
        if ( best != NULL && cities[ index ].g_val  <  best -> g_val )
        {
           /* If new path is better than the old one,
              then reset backpointer and update g_val and f_est. */
           ptr -> backptr = &cities[ index ];
           update_values( ptr );

           /* Is city on CLOSED? */
           if ( ptr -> closed == TRUE ) {
              /* If so, put back on OPEN. */
              ptr -> closed = FALSE;
              ptr -> open = TRUE;
              ++open_count;
           }
        }
}

void get_input()
{
        //FILE  *fptr;
        int   i, j;

        /* Read cities' names into structure variables. */
        for ( i = 0; i < SIZE; ++i )
        {
             int r = i / 5;
             int c = i % 5;

             cities[ i ].name[0] = '0' + r;
             cities[ i ].name[1] = '0' + c;
             if(r < goalRow)
             {
                if(c < goalColumn)
                {
                    cities[ i ].h_est = (goalRow - r) + (goalColumn - c);
                }

                else
                {
                    cities[ i ].h_est = (goalRow - r) + (c - goalColumn);
                }
             }

             else
             {
                if(c < goalColumn)
                {
                    cities[ i ].h_est = (r - goalRow) + (goalColumn - c);
                }

                else
                {
                    cities[ i ].h_est = (r - goalRow) + (c - goalColumn);
                }
             }

             cities[ i ].ind = i;
             cities[ i ].open = FALSE;
             cities[ i ].closed = FALSE;
        }

        /* Read distances between cities into matrix. */
        for ( i = 0; i < SIZE; ++i )
             for ( j = 0; j < SIZE; ++j )
             {
                distances[ i ][ j ] = 1;
             }

        /* Prompt user for START city, and read its index into START. */
}

void failure( void )
{
        Serial.print( "\n\n\n\tYou can't get there from here.\n\n " );
}

void success( const CITY *ptr )
{
        int i;

        Serial.print( "\n\n\n" );
        Serial.print( "\n\n\tOPTIMAL PATH LENGTH:\n" + String(ptr -> g_val));
        Serial.print( "\n\n\tOPTIMAL PATH:\n" );
        print_cities( ptr );

        // Serial.print( "\n\n\tEXPANDED VERTICES:\n" );
        // for ( i = 0; i < SIZE; ++i )
        // {
        //     if ( cities[ i ].closed == TRUE )
        //     {
        //       Serial.print( "\n\t" + String(cities[ i ].name[0]) + String (cities[ i ].name[1]));
        //       int number;
        //       char chaArr[2];
        //       chaArr[0] = cities[ i ].name[0];
        //       chaArr[1] = cities[ i ].name[1];
        //       //num[2] = '\n';
        //       //Serial.print(num[0] + " " + num[1]);
        //       number = atoi(chaArr);
        //       switch(number)
        //       {

        //         case 30: lightArr[lightIndex] = D0;
        //                   break;

        //         case 21: lightArr[lightIndex] = D1;
        //                   break;

        //         case 31: lightArr[lightIndex] = D2;
        //                   break;

        //         case 41: lightArr[lightIndex] = D3;
        //                   break;

        //         case 1: lightArr[lightIndex] = D4;
        //                   break;

        //         case 11: lightArr[lightIndex] = D5;
        //                   break;

        //         case 32: lightArr[lightIndex] = D6;
        //                   break;

        //         case 33: lightArr[lightIndex] = D7;
        //                   break;

        //         case 43: lightArr[lightIndex] = D8;
        //                   break;
        //       }
        //       lightIndex--;
        //     }

        //     lightLED();
        // }

        lightLED();

        Serial.print( "\n\n\n" );
}

void print_cities( const CITY *ptr )
{
        if ( ptr -> ind == START )            /* halt backtrack */
           Serial.print( "\n\t" + String(ptr -> name ));
        else {
           print_cities( ptr -> backptr );    /* backtrack */
           Serial.print( "\n\t" + String(ptr -> name));
        }

        int number = atoi(ptr -> name);
              switch(number)
              {

                case 30: lightArr[lightIndex] = D3;
                          break;

                case 21: lightArr[lightIndex] = D1;
                          break;

                case 31: lightArr[lightIndex] = D4;
                          break;

                case 41: lightArr[lightIndex] = D5;
                          break;

                case 11: lightArr[lightIndex] = D2;
                          break;

                case 32: lightArr[lightIndex] = D6;
                          break;

                case 33: lightArr[lightIndex] = D7;
                          break;

                case 43: lightArr[lightIndex] = D8;
                          break;
              }
              lightIndex--;

}

void lightLED()
{
  for(int j = 7; j >= 0; j--)
  {

    //Serial.println(String(lightArr[j]));
    if(lightArr[j] != -1)
    {
      //Serial.println(String(lightArr[i]));
      digitalWrite(lightArr[j], HIGH);
      delay(100);
      digitalWrite(lightArr[j], LOW);
    }

  }

  // digitalWrite(16, HIGH);
  // delay(500);
}

