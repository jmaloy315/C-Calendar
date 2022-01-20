/*Jack Maloy, 116329200, jmaloy*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "calendar.h"
#include "event.h"

/*This function initializes a calendar struct according to the values defined
in the parameters. Returns FAILURE if calendar is NULL, the number of days is
less than 1, or if the name of the calendar is NULL*/
int init_calendar(const char *name, int days, 
		  int (*comp_func) (const void *ptr1, const void *ptr2),
		  void (*free_info_func) (void *ptr), Calendar ** calendar){
  
  if((*calendar)==NULL || days < 1 || name==NULL){
    return FAILURE;
  }else{
    
    /*Allocating memory for the calendar, name, and events fields*/
    (*calendar) = malloc(sizeof(Calendar));
    if(calendar == NULL){
      return FAILURE;
    }
    (*calendar)->name = malloc(strlen(name)+1);
    strcpy((*calendar)->name, name);
    
    (*calendar)->events = calloc(days, sizeof(Event));
    
    (*calendar)->days = days;
    (*calendar)->total_events = 0;
    
    (*calendar)->comp_func = comp_func;
    (*calendar)->free_info_func = free_info_func;
    
    return SUCCESS;
  }
}

/*Prints the contents of a calendar to the designated output stream. If 
print_all is true, then this function also prints a which events are on which
day.*/
int print_calendar(Calendar *calendar, FILE *output_stream, int print_all){
  int i;
  Event *curr;
  
  /*Returns NULL if calendar or output_stream are NULL*/
  if(calendar==NULL || output_stream==NULL){
    return FAILURE;
  }else{
    if(print_all != 0){
      fprintf(output_stream, "Calendar's Name: \"%s\"\n", calendar->name);
      fprintf(output_stream,"Days: %d\n", calendar->days);
      fprintf(output_stream,"Total Events: %d\n\n", calendar->total_events);
    }
  
    printf("**** Events ****\n");
    if(calendar->total_events>0){
      for(i=0; i<calendar->days; i++){
	fprintf(output_stream,"Day %d\n", i+1);
	curr = calendar->events[i];
	while(curr!=NULL){
	  fprintf(output_stream, "Event's Name: \"%s\", ", curr->name);
	  fprintf(output_stream, "Start_time: %d, ", curr->start_time);
	  fprintf(output_stream, "Duration: %d\n", curr->duration_minutes);
	  curr = curr->next;
	}
      }
    }
    return SUCCESS;
  }
}

/*Adds an event to the specified calendar, to the specified day. The event is 
initialized with the fields described in the parameters.*/
int add_event(Calendar *calendar, const char *name, int start_time,
	      int duration_minutes, void *info, int day){

  Event *event, *marker, *prev;
  
  /*Fails if calendar or name is null, start_time is not an hour in the day, if
   the duration_minutes is less than 0, the day is less than 1 or if day is
  greater than the max days in calendar. */
  if(calendar==NULL || name==NULL || start_time>2400 || start_time<0 || 
     duration_minutes<0 || day<1 || day>calendar->days ||
     calendar->comp_func==NULL){
    return FAILURE;
  }else{

    /*Making a new event to be added.*/
    event = malloc(sizeof(Event));
    if(event==NULL){
      return FAILURE;
    }
    
    event->name = malloc(strlen(name)+1);
    
    strcpy(event->name, name);
    event->start_time = start_time;
    event->duration_minutes = duration_minutes;
    event->info = info;
   
    if(find_event(calendar, event->name, &event)==FAILURE){
      marker = calendar->events[day-1]; 
    
      /*If the day is empty*/
      if(marker == NULL){
	calendar->events[day-1] = event;
	event->next = marker;
	/*If the new events comp_func value is less than the first events*/
      }else if(calendar->comp_func(marker, event) >= 0){
	  calendar->events[day-1] = event;
	  event->next = marker;
      }else{
	/*adding to the middle or end of the list*/
	prev = marker;
	marker = marker->next;
	while(marker!=NULL && calendar->comp_func(marker, event)<=0){
	  prev = marker;
	  marker = marker->next;
	}
	event->next = marker;
	prev->next = event;
      }
    }
      (calendar->total_events)++;
      return SUCCESS;
    }
  return FAILURE;
}

/*finds an event with the given name in calendar. This event is stored in the 
event parameter if it is not NULL*/
int find_event(Calendar *calendar, const char *name, Event **event){
  int i;
  Event *marker;
  /*Fails if calendar or name are NULL*/
  if(calendar==NULL || name==NULL){
    return FAILURE;
  }else{
    for(i=0; i<calendar->days; i++){
      marker = calendar->events[i];
      if(marker!=NULL){
	if(strcmp(marker->name, name)==0){
	  if(event!=NULL){
	    *event = marker;
	  }
	  return SUCCESS;
	}else{
	  marker = marker->next;
	  while(marker!=NULL){
	    if(strcmp(marker->name, name)==0){
	      if(event!=NULL){
		*event = marker;
	      }
	      return SUCCESS;
	    }else{
	      marker = marker->next;
	    }
	  }
	}
      }
    }
    return FAILURE;
  }
}

/*finds an event in the calendar on a specified day. If the event is found and
is not NULL, it is stored in the event parameter.*/
int find_event_in_day(Calendar *calendar, const char *name, int day,
		      Event **event){
  Event *marker;
  if(calendar!=NULL && name!=NULL && day>0 && day<=calendar->days){
    marker = calendar->events[day-1];
    while(marker!=NULL && strcmp(marker->name, name)!=0){
      marker = marker->next;
    }
    if(marker!=NULL && event!=NULL){
      *event = marker;
      return SUCCESS;
    }
  }
  return FAILURE;
}

/*removes an event with the specified name from calendar. */
int remove_event(Calendar *calendar, const char *name){
  int i;
  Event *event = NULL, *prev, *curr;
  /*If the calendar or name are NULL, or if an event with the specified name is
   not found in the calendar, this function fails*/
  if(calendar!=NULL && name!=NULL && 
     find_event(calendar, name, &event)==FAILURE){
    
    for(i=0; i<calendar->days; i++){
      curr=calendar->events[i];
      
    if(curr != NULL){
	/*check head*/
	if(strcmp(curr->name, name)==0){
	  calendar->events[i] = curr->next;
	  curr->next = NULL;
	}else{
	  /*check rest of list*/
	  while(curr != NULL){
	    if(strcmp(curr->name, name)==0){
	      prev->next = curr->next;
	      curr->next = NULL;
	    }
	    prev = curr;
	    curr = curr->next;
	  }
	}
	/*free name, info, then the event itself*/
	free(curr->name);
	if(calendar->free_info_func!=NULL && curr->info!=NULL){
	  calendar->free_info_func(curr->info);
	}
	free(curr);
	(calendar->total_events)--;
	return SUCCESS;
      }
    }
  }
  return FAILURE;
}

/*returns a pointer to the info field of an event with name name, which is
inside calendar. */
void *get_event_info(Calendar *calendar, const char *name){
  Event *ret;
  if(find_event(calendar, name, &ret)==FAILURE){
    return NULL;
  }
  return ret->info;
}

/*Clears all events from the given day in calendar.*/
int clear_day(Calendar *calendar, int day){
  Event *cleaner, *marker;
  
  if(calendar==NULL || day<1 || day>calendar->days){
    return FAILURE;
  }else{
    marker = calendar->events[day-1];
    /*marker moves, cleaner follows and cleans every event*/
    while(marker != NULL){
      
      cleaner = marker;
      marker = marker->next;
      
      free(cleaner->name);
      if(calendar->free_info_func!=NULL && cleaner->info!=NULL){
	calendar->free_info_func(cleaner->info);
      }
      free(cleaner);
      (calendar->total_events)--;
    }
    /*clean the last event*/
    calendar->events[day-1] = NULL;
    (calendar->total_events)--;
    return SUCCESS;
  }
}

/*clears all events from all days in calendar*/
int clear_calendar(Calendar *calendar){
  int i;
  
  if(calendar==NULL){
    return FAILURE;
  }else{
    calendar->total_events = 0;
    for(i=0; i<calendar->days; i++){
      clear_day(calendar, i+1);
    }
    return SUCCESS;
  }
}

/*deallocates memory of all events and fields of calendar, and deallocates
 callendar*/
int destroy_calendar(Calendar *calendar){
  if(calendar==NULL){
    return FAILURE;
  }else{
    free(calendar->name);
    clear_calendar(calendar);
    free(calendar->events);
    free(calendar);
    return SUCCESS;
  }
}
