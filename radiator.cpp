#include "simlib.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <iostream>

int max_heat = 0;
double lost_temperature;
double out_temperature = 20;
int num_of_panel_energy_creation = 0;
double panel_efficiency = 1;
int consumed_el = 0;
double generated_el = 0;

struct node
{
    int temperature_min;
    int temperature_max;
    int num_of_days;
    double sunny_hours;
    int day_lenght;
    node *next;
};

class linked_list
{
public:
    node *head,*tail;
public:
    linked_list()
    {
        head = NULL;
        tail = NULL;
    }

    void add_node(int temperature_max, int temperature_min, double sunny_hours, int num_of_days, int day_lenght)
    {
        node *tmp = new node;
        tmp->temperature_min = temperature_min;
        tmp->temperature_max = temperature_max;
        tmp->sunny_hours = sunny_hours;
        tmp->num_of_days = num_of_days;
        tmp->day_lenght = day_lenght;
        tmp->next = NULL;

        if(head == NULL)
        {
            head = tmp;
            tail = tmp;
        }
        else
        {
            tail->next = tmp;
            tail = tail->next;
        }
    }
};
linked_list months;


class pump: public Process{
    void Behavior()
    {
        double cop;
        while (1)
        {
            cop = max_heat/(max_heat-out_temperature);
            consumed_el += lost_temperature/cop;
            Wait(1);
        }
    }
};

class panel_on: public Process{
    void Behavior()
    {
        while (1)
        {
            generated_el+=num_of_panel_energy_creation*panel_efficiency;
            Wait(1);
        }
    }
};

class weather: public Process{
    void Behavior()
    {
        node *tmp = months.head;
        int day_counter = 1;
        int hours_counter = 0;
        while (1)
        {
            if (hours_counter>8 && hours_counter<(8+tmp->day_lenght))   //day
            {       
                Wait(1);
                if (Random() <= tmp->sunny_hours)
                {
                    panel_efficiency = 1;
                }
                else
                {
                    panel_efficiency = Uniform(0.2,0.9);
                }

                if ( (int)(8+(tmp->day_lenght)/2)> hours_counter)
                {
                    out_temperature = Uniform(out_temperature,tmp->temperature_max);
                }
                else
                {
                    out_temperature = Uniform(tmp->temperature_min,out_temperature);
                }
                hours_counter++;
            }    
            else                                                        //night
            {
                Wait(1);
                if (7 <= hours_counter && 8+tmp->day_lenght+1 > hours_counter)
                {
                    out_temperature = Uniform(out_temperature,tmp->temperature_max);
                }
                else
                {
                    out_temperature = Uniform(tmp->temperature_min,out_temperature);
                }

                panel_efficiency = 0;
                hours_counter++;
            }

            if(hours_counter == 24)
            {
                day_counter++;
                hours_counter = 0;
            }

            if (day_counter == tmp->num_of_days)
            {
                if (tmp->next != NULL)
                {
                    tmp = tmp->next;
                    out_temperature = tmp->temperature_min+0.5;
                }
                day_counter = 1;
            }
        }
    }
};

int main(int argc, char *argv[])
{
    double g = 0;
    int area = 0;

    int opt;
    while((opt = getopt(argc, argv, "a:g:m:n:h")) != -1) 
    { 

        switch(opt) 
        { 
            case 'a': 
                if (atoi(optarg)<=0)
                {
                    printf("\nplocha nemôže byť nulová ani záporná (%i)\n",atoi(optarg));
                    return 0;
                }
                area = atoi(optarg);
                break; 
            case 'g': 
                g = std::__cxx11::stod(optarg);
                if (g<0.4 || g>2)
                    {
                        printf("\nhodnota G musí byť v rozsahu 0,4-2\n");
                        return 0;
                    }
                break; 
            case 'm': 
                max_heat = atoi(optarg);
                break;
            case 'n': 
                if (atoi(optarg)<0)
                {
                    printf("\nvýkon panela nemôže byť záporný (%i)\n",atoi(optarg));
                    return 0;
                }
                num_of_panel_energy_creation = atoi(optarg);
                break;
            case 'h':
                printf("\n./radiator [-a <rozloha domu>] [-g <x>] [-m <<vykurovacia teplota>] [-n <maximálny výkon panelu>] [-h]\n");
                printf("-a <rozloha domu> -- veľkosť domu v m2\n");
                printf("-g <x>: \n");
                printf("    x--starý dom bez izolácie:                    -> 2\n");
                printf("    x--starý dom dodatočne čiastočne izolovaný:   -> 1.5\n");
                printf("    x--dom postavený po roku 1990:                -> 1.1\n");
                printf("    x--dom postavený po roku 2005:                -> 0.8\n");
                printf("    x--dom postavený v rokoch 2010 - 2015:        -> 0.6\n");
                printf("    x--dom postavený po roku 2015:                -> 0.4\n");
                printf("-m <vykurovacia teplota>  --teplota(C°) ktorá má byť udržovaná vnútry domu\n");
                printf("-n <maximálny výkon panelu> -- maximálny výkon panelu(W) počas jasnej oblohy\n");
                printf("-h zobrazí nápovedu\n");
                return 0; 
            case '?':
                return -1;
        } 
    }

    if((area == 0)||(g == 0)||(max_heat == 0))
    {
        printf("\n./radiator [-a <rozloha domu>] [-g <x>] [-m <<vykurovacia teplota>] [-n <maximálny výkon panelu>] [-h]\n");
        printf("-a <rozloha domu> -- veľkosť domu v m2\n");
        printf("-g <x>: \n");
        printf("    x--starý dom bez izolácie:                    -> 2\n");
        printf("    x--starý dom dodatočne čiastočne izolovaný:   -> 1.5\n");
        printf("    x--dom postavený po roku 1990:                -> 1.1\n");
        printf("    x--dom postavený po roku 2005:                -> 0.8\n");
        printf("    x--dom postavený v rokoch 2010 - 2015:        -> 0.6\n");
        printf("    x--dom postavený po roku 2015:                -> 0.4\n");
        printf("-m <vykurovacia teplota>  --teplota(C°) ktorá má byť udržovaná vnútry domu\n");
        printf("-n <maximálny výkon panelu> -- maximálny výkon panelu(W) počas jasnej oblohy\n");
        printf("-h zobrazí nápovedu\n");
        return 0; 
    }
    lost_temperature = g*(area*2.45)*(max_heat+2);

    months.add_node(20,10,0.512,30,13);     //sep
    months.add_node(15,7,0.440,31,11);      //oct
    months.add_node(10,3,0.204,30,9);       //nov
    months.add_node(2,-1,0.202,31,8);       //dec

    months.add_node(13,-3,0.197,31,9);       //jan
    months.add_node(10,0,0.304,28,10);       //feb
    months.add_node(15,-1,0.403,31,12);      //mar
    months.add_node(16,3,0.476,30,14);       //apr
    months.add_node(22,14,0.591,31,15);      //may

    RandomSeed(time(NULL));
    Init(0,6551);

    (new pump)->Activate();
    (new panel_on)->Activate();
    (new weather)->Activate();

    Run();

    printf("\nconsumed electricity: %dKw\n", consumed_el/1000);
    if(num_of_panel_energy_creation != 0)
    {
        printf("\ngenerated electricity: %fKw\n", generated_el/1000);
        if (consumed_el-generated_el<0)
        {
            printf("\nelectricity stored in virtual battery: %fKw\n", (consumed_el-generated_el)/1000*(-1));
        }
        else
            printf("\nelectricity to be payed: %fKw\n", (consumed_el-generated_el)/1000);
    }
    return 0;}