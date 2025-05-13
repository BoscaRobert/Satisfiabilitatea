#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#define MAX_LIT 100000
#include<sys/resource.h>
#include<time.h>

void afisare_perf()
{
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    printf("%ld, %ld\n", usage.ru_maxrss,clock());
}

struct clauza
{
    int marime;
    int *literali;
};
struct literal
{
    unsigned char POZ;
    unsigned char NEG;
};

int citire_clauze(struct clauza *lista_clauze)
{
    int i=0,k=0,n,negativ;
    char aux;
    int temp[MAX_LIT];
    FILE *fisier = fopen("Clauze_DP.txt","r");
    while(true)
    {
        i=0;
        while(true)
        {
            n=0;
            negativ=0;
            while(true)
            {
                aux=getc(fisier);
                //printf("%c",aux);
                if(aux==EOF)
                {
                    goto Sfarsit_Fisier;
                }
                else if(aux==' ')
                {
                    goto al;
                }
                else if(aux=='\n')
                {
                    goto ac;
                }
                else if(aux=='-')
                {
                    negativ=1;
                }
                else
                {
                    n=n*10+aux-'0';
                }
            }
            al:
            temp[i]=n*(1-2*negativ);
            i++;
        }
        ac:
        temp[i]=n*(1-2*negativ);
        i++;
        lista_clauze[k].literali=(int*)malloc(sizeof(int)*i);
        for(int j=0;j<i;j++)
        {
            lista_clauze[k].literali[j]=temp[j];
        }
        lista_clauze[k].marime=i;
        k++;
        i=0;
    }
    Sfarsit_Fisier:
    return k;
}

void afisare_clauze(struct clauza *lista_clauze,int dimensiune)
{
    for(int i=0;i<dimensiune;i++)
    {
        for(int j=0;j<lista_clauze[i].marime;j++)
           {
            printf("%d ",lista_clauze[i].literali[j]);
           }
        printf("\n");

    }        
}

int cel_mai_frecvent(struct clauza *lista_clauze,int dimensiune)
{
    int maxim=0,aux,cmf=0,r=0;
    int frecventa_poz[MAX_LIT]={0};
    int frecventa_neg[MAX_LIT]={0};
    for(int i=0;i<dimensiune;i++)
    {
        for(int j=0;j<lista_clauze[i].marime;j++)
           {
            aux=lista_clauze[i].literali[j];
            if(aux>0)
                frecventa_poz[aux]++;
            else
                frecventa_neg[abs(aux)]++;
            if(-aux>maxim)
                maxim=-aux;
           }
    }   
    for(int i=0;i<=maxim;i++)
    {
        if(frecventa_poz[i]>cmf)
        {
            cmf=frecventa_poz[i];
            r=i;
        }
        if(frecventa_neg[i]>cmf)
        {
            cmf=frecventa_neg[i];
            r=-i;
        }
    }
    return r;
}

void sterge_literal(struct clauza *lista_clauze,int clauza,int literalul)
{
    int index=0;
    int temp[MAX_LIT];
    for(int *i=lista_clauze[clauza].literali   ;   i<lista_clauze[clauza].literali+lista_clauze[clauza].marime   ;   i++)
    {
        if(*i!=literalul)
        {
            temp[index]=*i;
            ++index;
        }
    }
    lista_clauze[clauza].marime=index;
    free(lista_clauze[clauza].literali);
    lista_clauze[clauza].literali=(int*)malloc(sizeof(int)*index);
    index=0;
    for(int *i=lista_clauze[clauza].literali   ;   i<lista_clauze[clauza].literali+lista_clauze[clauza].marime   ;   i++)
    {
        *i=temp[index];
        index++;
    }
}
void stergere_clauza(struct clauza *lista_clauze,int clauza,int *dimensiune)
{
    int index=0;
    free(lista_clauze[clauza].literali);
    int temp[MAX_LIT];
    for(int i=0;i<lista_clauze[*dimensiune-1].marime;i++)
    {
        temp[i]=lista_clauze[*dimensiune-1].literali[i];
        ++index;
    }
    //free(lista_clauze[*dimensiune-1].literali);
    lista_clauze[clauza].literali=(int*)malloc(sizeof(int)*index);
    for(int i=0;i<index;i++)
    {
        lista_clauze[clauza].literali[i]=temp[i];
    }
    --*dimensiune;  
    lista_clauze[clauza].marime=index;
    //lista_clauze=(struct clauza*)realloc(lista_clauze,*dimensiune);
}

int contine_literalul(struct clauza *lista_clauze,int clauza,int literal)
{
    for(int i=0;i<lista_clauze[clauza].marime;i++)
    {
        if(lista_clauze[clauza].literali[i]==literal)
        {
            return 1;
            break;
        }
    }
    return 0;
}

int propagare(struct clauza *lista_clauze,int *dimensiune)
{
    int unitar;
    for(int i=0;i<*dimensiune;i++)
    {
        if (lista_clauze[i].marime==1)
        {
            unitar=lista_clauze[i].literali[0];
            for(int j=0;j<*dimensiune;j++)
            {
                if(contine_literalul(lista_clauze,j,unitar))
                {
                    stergere_clauza(lista_clauze,j,dimensiune);
                }
                if(contine_literalul(lista_clauze,j,0-unitar))
                {
                    sterge_literal(lista_clauze,j,0-unitar);
                }
            }
            return 1;
            break;
        }
    }
    return 0;
}

void gasire_literali_puri(struct clauza *lista_clauze,int dimensiune,int puri[MAX_LIT],int *marime_puri)
{
    struct literal frecventa[MAX_LIT]={0};
    int lit,maxim=0;
    for(int i=0;i<dimensiune;i++)
    {
        for(int j=0;j<lista_clauze[i].marime;j++)
        {
            lit=lista_clauze[i].literali[j];
            if(lit>0)
            {
                frecventa[lit].POZ=1;
            }
            else
            {
                frecventa[0-lit].NEG=1;
            }
            if(abs(lit)>maxim)
               maxim=abs(lit);
        }
    }
    int k=0;
    for(int i=0;i<maxim;i++)
    {
        if(frecventa[i].NEG==1&&frecventa[i].POZ==0)
           {
            puri[k]=-i;
            k++;
           }
        else if (frecventa[i].NEG==0&&frecventa[i].POZ==1)
        {
            puri[k]=i;
            k++;
        }
    }
    *marime_puri=k;
}

int stergere_literali_puri(struct clauza *lista_clauze, int *dimensiune)
{
    int puri[MAX_LIT],marime_puri,stersi=0;
    gasire_literali_puri(lista_clauze,*dimensiune,puri,&marime_puri);
    for(int i=0;i<marime_puri;i++)
    {
        for(int j=0;j<*dimensiune;j++)
        {
            if(contine_literalul(lista_clauze,j,puri[i]))
            {
                stergere_clauza(lista_clauze,j,dimensiune);
                j--;
            }
        }
    }
    if(marime_puri>0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void sterge_tautologiile(struct clauza *lista_clauze, int *dimensiune)
{
    int k=0;
    for(int i=0;i<*dimensiune;i++)
    {
        struct literal lit[MAX_LIT]={0,0};
        k=0;
        for(int j=0;j<lista_clauze[i].marime;j++)
        {
            if(lista_clauze[i].literali[j]>0)
            {
                lit[lista_clauze[i].literali[j]].POZ=1;
                if((lit[lista_clauze[i].literali[j]].POZ==1)&&(lit[lista_clauze[i].literali[j]].NEG==1))
                {
                    stergere_clauza(lista_clauze,i,dimensiune);
                    --i;
                }
            }
            else
            {
                lit[0-lista_clauze[i].literali[j]].NEG=1;
                if((lit[0-lista_clauze[i].literali[j]].POZ==1)&&(lit[0-lista_clauze[i].literali[j]].NEG==1))
                {
                    stergere_clauza(lista_clauze,i,dimensiune);
                    --i;
                }
            }
        }
    }
}
void adauga(struct clauza **lista_clauze, int *dimensiune, struct clauza c)
{
    struct clauza *temp = (struct clauza*)realloc(*lista_clauze, (*dimensiune + 1) * sizeof(struct clauza));
    *lista_clauze = temp;
    (*lista_clauze)[*dimensiune] = c;
    *dimensiune += 1;
}


void rezolutie(struct clauza *lista_clauze, int *dimensiune,int literal)
{
    afisare_perf();
    for(int i=0;i<*dimensiune;i++)
    {
        if(contine_literalul(lista_clauze,i,literal))
        {
            for(int j=0;j<*dimensiune;j++)
            {
                if(contine_literalul(lista_clauze,j,0-literal))
                {
                    struct clauza c;
                    c.literali=(int*)malloc(sizeof(int)*(lista_clauze[i].marime+lista_clauze[j].marime));
                    int index=0;
                    for(int k=0;k<lista_clauze[i].marime;k++)
                    {
                        if((lista_clauze[i].literali[k]!=literal)&&(lista_clauze[i].literali[k]!=0-literal))
                        {
                            c.literali[index]=lista_clauze[i].literali[k]; 
                            index++;
                        }
                    }
                    for(int k=0;k<lista_clauze[j].marime;k++)
                    {
                        if((lista_clauze[j].literali[k]!=literal)&&(lista_clauze[j].literali[k]!=0-literal))
                        {
                            c.literali[index]=lista_clauze[j].literali[k];
                            index++;
                        }
                    }
                    c.marime=index;
                    adauga(&lista_clauze,dimensiune,c);
                    
                }
            }
        }
    }
    for(int i=0;i<*dimensiune;i++)
    {
        if(contine_literalul(lista_clauze,i,literal)||(contine_literalul(lista_clauze,i,0-literal)))
        {
            stergere_clauza(lista_clauze,i,dimensiune);
        }
    }
}

int main()
{
    clock_t timp=clock();
    int n,pur=1;
    struct clauza *lista_clauze=(struct clauza*)malloc(sizeof(struct clauza)*MAX_LIT);
    int dimensiune=citire_clauze(lista_clauze);
    afisare_clauze(lista_clauze,dimensiune);
    while(true)
    {
        if(dimensiune==0)
        {
            printf("\n%d\n",clock()-timp);
            goto Satisfiabil;
        }
        for(int i=0;i<dimensiune;i++)
        {
            if(lista_clauze[i].marime==0)
            {
                printf("\n%d\n",clock()-timp);
                goto Nesatisfiabil;
            }
        }
        rezolutie(lista_clauze,&dimensiune,lista_clauze[0].literali[0]);
    }
    Satisfiabil:
    printf("satisfiabil");
    goto END;
    Nesatisfiabil:
    printf("nesatisfiabil");
    END: 
    timp=clock()-timp;
    return 0;
}