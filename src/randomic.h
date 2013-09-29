#ifndef RANDOMIC_H_
#define RANDOMIC_H_

typedef struct randomic TRandomic;
typedef unsigned int TPickedRandomic;
typedef TPickedRandomic (* TPickRandomic)(TRandomic *randomic);
typedef TPickedRandomic (* TLastRandomic)(TRandomic *randomic);
typedef void (* TResetRandomic)(TRandomic *randomic, char *);
typedef void (* TDisposeRandomic)(TRandomic *randomic);

struct randomic{
	// private data
    void *data;
    //public methods
    TPickRandomic pick;
    TLastRandomic last;
    TResetRandomic reset;
    TDisposeRandomic dispose;
};

TRandomic *createRandomic();
void disposeRandomic(TRandomic *);

// Randomic's variables

//constant
TRandomic *createConstantRandomic(char *entry);
TRandomic *createSingletonConstantRandomic(char *entry);

//Pareto variates
//TPickRandomic *initPickedPareto(unsigned short seed[], float alpha, float beta);
TRandomic *createParetoRandomic(char *entry);
TRandomic *createSingletonParetoRandomic(char *entry);

//Exponential variates
TRandomic *createExponentialRandomic(char *entry);
TRandomic *createSingletonExponentialRandomic(char *entry);

//TruncatedGeometric variates
TRandomic *createTruncatedGeometricRandomic(char *entry);
TRandomic *createSingletonTruncatedGeometricRandomic(char *entry);

//Uniform variates
TRandomic *createUniformRandomic(char *entry);
TRandomic *createSingletonUniformRandomic(char *entry);

//Poisson variates
TRandomic *createPoissonRandomic(char *entry);
TRandomic *createSingletonPoissonRandomic(char *entry);

//Zipf variates
TRandomic *createZipfRandomic(char *entry);
TRandomic *createSingletonZipfRandomic(char *entry);

//Lognormal variates
TRandomic *createLognormalRandomic(char *entry);
TRandomic *createSingletonLognormalRandomic(char *entry);

//From file
TRandomic *createFromFileRandomic(char *fileName);
TRandomic *createSingletonFromFileRandomic(char *fileName);

//Binomial pick
TRandomic *createBinomialRandomic(char *entry);
TRandomic *createSingletonBinomialRandomic(char *entry);


#endif /* RANDOMIC_H_ */
