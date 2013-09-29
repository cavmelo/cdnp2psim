#include "time.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "assert.h"
#include "string.h"
#include "randomic.h"
#include "internals.h"
#include "dictionary.h"


typedef struct _dataRandomic TDataRandomic;
struct _dataRandomic{
    unsigned short seed[3];
    unsigned int pick;
    void *pars;
};

TRandomic *createRandomic(){
    TRandomic *p = (TRandomic *)malloc(sizeof(TRandomic));
    p->data = NULL;
    p->dispose = disposeRandomic;
    p->last = NULL;
    p->pick = NULL;
    p->reset = NULL;

    return p;

}

void disposeRandomic(TRandomic *randomic){

	if(randomic){
		TDataRandomic *p = randomic->data;
		if (p){
			free(p->pars);
			free(p);
		}
		free(randomic);
	}
}

static void setSeedRandomic(unsigned short seed[]){
	srand48(time(NULL));

	seed[0] = seed[0]+(drand48()*8192);
	seed[1] = seed[1]+(drand48()*8192);
	seed[2] = seed[2]+(drand48()*8192);

}


typedef struct _dataParetoRandomic TDataParetoRandomic;
struct _dataParetoRandomic{
    float beta;
    float alpha;
};

TPickedRandomic pickParetoRandomic(TRandomic *randomic);
TPickedRandomic getLastPickedParetoRandomic(TRandomic *randomic);


TRandomic *createParetoRandomic(char *entry){
	static unsigned short seed[] = {10,11,12};

    TRandomic *p = createRandomic();
    TDataRandomic *data = (TDataRandomic *) malloc(sizeof(TDataRandomic));
    TDataParetoRandomic *pars = (TDataParetoRandomic *) malloc(sizeof(TDataParetoRandomic));

    // extract parameters from user's entry limited by semi-color
    TParameters *lp = createParameters(entry, PARAMETERS_SEPARATOR);

    lp->iterator(lp);


    // walk through the parameters as setting up
    pars->alpha = atof(lp->next(lp));
    pars->beta = atof(lp->next(lp));

	setSeedRandomic(seed);

    data->seed[0] = seed[0];
    data->seed[1] = seed[1];
    data->seed[2] = seed[2];

    data->pars = pars;

    p->data = data;
    p->pick = pickParetoRandomic;
    p->last = getLastPickedParetoRandomic;


    lp->dispose(lp);

    return p;
}


TPickedRandomic pickParetoRandomic(TRandomic *randomic){

	TDataRandomic *p = (TDataRandomic *)randomic->data;
	TDataParetoRandomic *pars = (TDataParetoRandomic*)p->pars;

    p->pick = (TPickedRandomic)(pars->beta*pow(1.0 / (1.0 - erand48(p->seed) ), 1.0/pars->alpha ));

    return p->pick;
}

TPickedRandomic getLastPickedParetoRandomic(TRandomic *randomic){
    TDataRandomic *p = (TDataRandomic *)randomic->data;

    return p->pick;
}

TRandomic *createSingletonParetoRandomic(char *entry){
	static TRandomic *p=NULL;
	if (!p)
		p = createParetoRandomic(entry);
	return p;
}

// Constant variate
typedef struct _dataConstantRandomic TDataConstantRandomic;
struct _dataConstantRandomic{
    unsigned int mean;
};

TPickedRandomic pickConstantRandomic(TRandomic *randomic);
TPickedRandomic getLastPickedConstant(TRandomic *randomic);

TRandomic *createConstantRandomic(char *entry){
    TRandomic *p = createRandomic();
    TDataRandomic *data = (TDataRandomic *) malloc(sizeof(TDataRandomic));
    TDataConstantRandomic *pars = (TDataConstantRandomic *) malloc(sizeof(TDataConstantRandomic));

    // extract parameters from user's entry limited by semi-color
    TParameters *lp = createParameters(entry, PARAMETERS_SEPARATOR);

    lp->iterator(lp);

    // walk through the parameters as setting up
    pars->mean = atoi(lp->next(lp));

    data->pars = pars;

    p->data = data;
    p->pick = pickConstantRandomic;
    p->last = getLastPickedConstant;

    lp->dispose(lp);

    return p;
}

TPickedRandomic pickConstantRandomic(TRandomic *randomic){
    TDataRandomic *p = (TDataRandomic *)randomic->data;
    TDataConstantRandomic *pars = (TDataConstantRandomic *)p->pars;

    p->pick = pars->mean;

    return p->pick;
}

TPickedRandomic getLastPickedConstant(TRandomic *randomic){
    TDataRandomic *p = (TDataRandomic *)randomic->data;

    return p->pick;
}

TRandomic *createSingletonConstantRandomic(char *entry){
	static TDictionary *d=NULL;
	TRandomic *p=NULL;
	TKeyDictionary key;
	if (!d)
		d = createDictionary();

	key = d->keyGenesis(entry);
	if (d->has(d,key)){
		p = d->retrieval(d,key);
	}else{
		p = createConstantRandomic(entry);
		d->insert(d,key,p);
	}

	return p;
}


//@ Exponential Related data and functions
typedef struct _dataExponentialRandomic TDataExponentialRandomic;
struct _dataExponentialRandomic{
    float mean;
};

TPickedRandomic pickExponentialRandomic(TRandomic *randomic);
TPickedRandomic getLastPickedExponential(TRandomic *randomic);
void resetExponentialRandomic(TRandomic *randomic, char* entry);

TRandomic *createExponentialRandomic(char *entry){
	static unsigned short seed[]={10,12,14};

	TRandomic *p = createRandomic();
	TDataRandomic *data = data = (TDataRandomic *) malloc(sizeof(TDataRandomic));
    TDataExponentialRandomic *pars = (TDataExponentialRandomic *)malloc(sizeof(TDataExponentialRandomic));

    TParameters *lp = createParameters(entry, PARAMETERS_SEPARATOR);

    setSeedRandomic(seed);

    lp->iterator(lp);

    pars->mean = atof(lp->next(lp));

    
	setSeedRandomic(seed);

    data->seed[0] = seed[0];
    data->seed[1] = seed[1];
    data->seed[2] = seed[2];

    data->pars = pars;
    
    p->data = data;
    p->pick = pickExponentialRandomic;
    p->last = getLastPickedExponential;
    p->reset = resetExponentialRandomic;

    lp->dispose(lp);

    return p;
}


void resetExponentialRandomic(TRandomic *randomic, char *entry){
    TDataRandomic *p = (TDataRandomic *)randomic->data;
    TDataExponentialRandomic *pars = (TDataExponentialRandomic *)p->pars;

    TParameters *lp = createParameters(entry, PARAMETERS_SEPARATOR);

    lp->iterator(lp);

    pars->mean = atof(lp->next(lp));

    lp->dispose(lp);
}

TPickedRandomic pickExponentialRandomic(TRandomic *randomic){
    TDataRandomic *p = (TDataRandomic *)randomic->data;
    TDataExponentialRandomic *pars = (TDataExponentialRandomic *)p->pars;

    p->pick = (TPickedRandomic)(1 + (TPickedRandomic)(-log( erand48(p->seed) ) * pars->mean));

    return p->pick;
}

TPickedRandomic getLastPickedExponential(TRandomic *randomic){
    TDataRandomic *p = (TDataRandomic *)randomic->data;

    return p->pick;
}

TRandomic *createSingletonExponentialRandomic(char *entry){
	static TDictionary *d=NULL;
	TRandomic *p=NULL;
	TKeyDictionary key;
	if (!d)
		d = createDictionary();

	key = d->keyGenesis(entry);
	if (d->has(d,key)){
		p = d->retrieval(d,key);
	}else{
		p = createExponentialRandomic(entry);
		d->insert(d,key,p);
	}
	return p;
}

//@ TruncatedGeometric Related data and functions
typedef struct _dataTruncatedGeometricRandomic TDataTruncatedGeometricRandomic;
struct _dataTruncatedGeometricRandomic{
    float alpha;
    int B;
};

TPickedRandomic pickTruncatedGeometricRandomic(TRandomic *randomic);
TPickedRandomic getLastPickedTruncatedGeometric(TRandomic *randomic);
void resetTruncatedGeometricRandomic(TRandomic *randomic, char *entry);

TRandomic *createTruncatedGeometricRandomic(char *entry){
	static unsigned short seed[]={10,12,14};

	TRandomic *p = createRandomic();
	TDataRandomic *data = data = (TDataRandomic *) malloc(sizeof(TDataRandomic));
    TDataTruncatedGeometricRandomic *pars = (TDataTruncatedGeometricRandomic *)malloc(sizeof(TDataTruncatedGeometricRandomic));

//    setSeedRandomic(seed);

    TParameters *lp = createParameters(entry, PARAMETERS_SEPARATOR);

    lp->iterator(lp);

    pars->alpha = atof(lp->next(lp));

    
	setSeedRandomic(seed);

    data->seed[0] = seed[0];
    data->seed[1] = seed[1];
    data->seed[2] = seed[2];

    data->pars = pars;
    
    p->data = data;
    p->pick = pickTruncatedGeometricRandomic;
    p->last = getLastPickedTruncatedGeometric;
    p->reset = resetTruncatedGeometricRandomic;

    lp->dispose(lp);

    return p;
}

void resetTruncatedGeometricRandomic(TRandomic *randomic, char *entry){
    TDataRandomic *p = (TDataRandomic *)randomic->data;
    TDataTruncatedGeometricRandomic *pars = (TDataTruncatedGeometricRandomic *)p->pars;

    TParameters *lp = createParameters(entry, PARAMETERS_SEPARATOR);

    lp->iterator(lp);

    pars->B = atoi(lp->next(lp));

    lp->dispose(lp);
}

TPickedRandomic pickTruncatedGeometricRandomic(TRandomic *randomic){
    TDataRandomic *p = (TDataRandomic *)randomic->data;
    TDataTruncatedGeometricRandomic *pars = (TDataTruncatedGeometricRandomic *)p->pars;

	double u = erand48(p->seed);
	double cumProb = 0;

	unsigned int n = 0;
	double q = 1; // probability that value is >= n
	if (pars->B < 0){
		fprintf(stderr,"ERROR: RANDOM: pickTruncatedGeometric\n");
		exit(0);
	}

	while (1) {

	    if (n == pars->B) {
		    break;
	    }

	    cumProb += ((1 - pars->alpha) * q);
	    if (cumProb > u) {
		   break;
	    }

	    ++n;
	    q *= pars->alpha;
	}

	p->pick = n;
    

    return p->pick;
}

TPickedRandomic getLastPickedTruncatedGeometric(TRandomic *randomic){
    TDataRandomic *p = (TDataRandomic *)randomic->data;

    return p->pick;
}

TRandomic *createSingletonTruncatedGeometricRandomic(char *entry){
	static TDictionary *d=NULL;
	TRandomic *p=NULL;
    	TKeyDictionary key;
	if (!d)
		d = createDictionary();

	key = d->keyGenesis(entry);
	if (d->has(d,key)){
		p = d->retrieval(d,key);
	}else{
		p = createTruncatedGeometricRandomic(entry);
		d->insert(d,key,p);
	}
	return p;
}


//Uniform variate
typedef struct _dataUniformRandomic TDataUniformRandomic;
struct _dataUniformRandomic{
    int range;
};

TPickedRandomic pickUniformRandomic(TRandomic *randomic);
TPickedRandomic getLastPickedUniformRandomic(TRandomic *randomic);
void resetUniformRandomic(TRandomic *randomic, char *entry);

TRandomic *createUniformRandomic( char *entry ){
	static unsigned short seed[]={12,1,2};

    TRandomic *p = createRandomic();
    TDataRandomic *data = (TDataRandomic *) malloc(sizeof(TDataRandomic));
    TDataUniformRandomic *pars = (TDataUniformRandomic *)malloc(sizeof(TDataUniformRandomic));

    TParameters *lp = createParameters(entry, PARAMETERS_SEPARATOR);

    lp->iterator(lp);

    pars->range = atof(lp->next(lp));

    data = (TDataRandomic *) malloc(sizeof(TDataRandomic));

	setSeedRandomic(seed);
    data->seed[0] = seed[0];
    data->seed[1] = seed[1];
    data->seed[2] = seed[2];

    data->pars = pars;

    p->data = data;
    p->pick = pickUniformRandomic;
    p->last = getLastPickedUniformRandomic;
    p->reset = resetUniformRandomic;

    lp->dispose(lp);

    return p;
}

    
TPickedRandomic pickUniformRandomic(TRandomic *randomic){
    TDataRandomic *p = (TDataRandomic *)randomic->data;
    TDataUniformRandomic *pars = (TDataUniformRandomic*)p->pars;
    
    p->pick = (TPickedRandomic)(1 + (TPickedRandomic)(pars->range * erand48(p->seed)));
    
    return p->pick;
}

TPickedRandomic getLastPickedUniformRandomic(TRandomic *randomic){
    TDataRandomic *p = (TDataRandomic *)randomic->data;
    
    return p->pick;
}

void resetUniformRandomic( TRandomic *randomic, char *entry ){
    TDataRandomic *data = (TDataRandomic *)randomic->data;
    TDataUniformRandomic *pars = (TDataUniformRandomic*)data->pars;

    TParameters *lp = createParameters(entry, PARAMETERS_SEPARATOR);

    lp->iterator(lp);

    pars->range = atof(lp->next(lp));

    data->pars = pars;

    lp->dispose(lp);

}

TRandomic *createSingletonUniformRandomic(char *entry){
	static TDictionary *d=NULL;
	TRandomic *p=NULL;
	TKeyDictionary key;
	if (!d)
		d = createDictionary();

	key = d->keyGenesis(entry);
	if (d->has(d,key)){
		p = d->retrieval(d,key);
	}else{
		p = createUniformRandomic(entry);
		d->insert(d,key,p);
	}

	return p;
}


// Poisson variates
typedef struct _dataPoissonRandomic TDataPoissonRandomic;
struct _dataPoissonRandomic{
    int mean;
};

TPickedRandomic pickPoissonRandomic(TRandomic *randomic);
TPickedRandomic getLastPickedPoissonRandomic(TRandomic *randomic);

TRandomic *createPoissonRandomic(char *entry){
	static unsigned short seed[]={1,12,45};

    TRandomic *p = createRandomic();
    TDataRandomic *data = (TDataRandomic *) malloc(sizeof(TDataRandomic));

    TDataPoissonRandomic *pars = (TDataPoissonRandomic *)malloc(sizeof(TDataPoissonRandomic));

    TParameters *lp = createParameters(entry, PARAMETERS_SEPARATOR);

    setSeedRandomic(seed);

    lp->iterator(lp);

    pars->mean = atof(lp->next(lp));


    data->seed[0] = seed[0];
    data->seed[1] = seed[1];
    data->seed[2] = seed[2];


    data->pars = pars;

    p->data = data;
    p->pick = pickPoissonRandomic;
    p->last = getLastPickedPoissonRandomic;

    lp->dispose(lp);

    return p;
}

TPickedRandomic pickPoissonRandomic(TRandomic *randomic){
    TDataRandomic *p = (TDataRandomic *)randomic->data;
    TDataPoissonRandomic *pars = (TDataPoissonRandomic *)p->pars;

    // by Knuth
    // TO-DO check sanity
    double L = (double)exp((double)-(pars->mean));
    int k = 0;
    double pick = 1.0;
    do{
		k  =  k + 1;
		//Generate uniform random number u in [0,1] and let p ← p × u.
		pick = pick * erand48(p->seed);
	}while (pick > L);

    p->pick = (TPickedRandomic) (k-1);

    return p->pick;
}

TPickedRandomic getLastPickedPoissonRandomic(TRandomic *randomic){
    TDataRandomic *p = (TDataRandomic *)randomic->data;


    return p->pick;
}

TRandomic *createSingletonPoissonRandomic(char *entry){
	static TDictionary *d=NULL;
	TRandomic *p=NULL;
	TKeyDictionary key;
	if (!d)
		d = createDictionary();

	key = d->keyGenesis(entry);
	if (d->has(d,key)){
		p = d->retrieval(d,key);
	}else{
		p = createPoissonRandomic(entry);
		d->insert(d,key,p);
	}
	return p;
}


//Binomial distribution
//
typedef struct _dataBinomialRandomic TDataBinomialRandomic;
struct _dataBinomialRandomic{
    int trials;
    float sucess;
};

TPickedRandomic pickBinomialRandomic(TRandomic *randomic);
TPickedRandomic getLastPickedBinomialRandomic(TRandomic *randomic);

TRandomic *createBinomialRandomic(char *entry){
	static unsigned short seed[]={24,89,9};

    TRandomic *p = createRandomic();
    TDataRandomic *data = (TDataRandomic *) malloc(sizeof(TDataRandomic));
    TDataBinomialRandomic *pars = (TDataBinomialRandomic *)malloc(sizeof(TDataBinomialRandomic));

    TParameters *lp = createParameters(entry, PARAMETERS_SEPARATOR);

    setSeedRandomic(seed);

    lp->iterator(lp);

    pars->trials = atoi(lp->next(lp));;
    pars->sucess = atof(lp->next(lp));;

    data->seed[0] = seed[0];
    data->seed[1] = seed[1];
    data->seed[2] = seed[2];


    data->pars = pars;

    p->data = data;
    p->pick = pickBinomialRandomic;
    p->last = getLastPickedBinomialRandomic;

    lp->dispose(lp);

    return p;
}

TPickedRandomic pickBinomialRandomic(TRandomic *randomic){
    TDataRandomic *p = (TDataRandomic *)randomic->data;
    TDataBinomialRandomic *pars = (TDataBinomialRandomic *)p->pars;

    // by Knuth
    // TO-DO check sanity
    int k=0, i;
    for(i=0;i<pars->trials;i++){
		if (erand48(p->seed)<pars->sucess)
			k++;
	}

    p->pick = (TPickedRandomic) (k);

    return p->pick;
}

TPickedRandomic getLastPickedBinomialRandomic(TRandomic *randomic){
    TDataRandomic *p = (TDataRandomic *)randomic->data;

    return p->pick;
}

TRandomic *createSingletonBinomialRandomic(char *entry){
	static TDictionary *d=NULL;
	TRandomic *p=NULL;
	TKeyDictionary key;
	if (!d)
		d = createDictionary();

	key = d->keyGenesis(entry);
	if (d->has(d,key)){
		p = d->retrieval(d,key);
	}else{
		p = createBinomialRandomic(entry);
		d->insert(d,key,p);
	}

	return p;
}

//:LogNormal Variate
typedef struct _dataLogNormalRandomic TDataLogNormalRandomic;
struct _dataLogNormalRandomic{
    double sigma;
    double mu;
    unsigned int rank;
    unsigned int pick;
};

TPickedRandomic pickLognormalRandomic(TRandomic *randomic);
TPickedRandomic getLastPickedLognormalRandomic(TRandomic *randomic);

TRandomic *createLognormalRandomic(char *entry){
	static unsigned short seed[]={129, 15, 18};

    TRandomic *p = createRandomic();
    TDataRandomic *data = (TDataRandomic *) malloc(sizeof(TDataRandomic));
    TDataLogNormalRandomic *pars = (TDataLogNormalRandomic *)malloc(sizeof(TDataLogNormalRandomic));

    TParameters *lp = createParameters(entry,PARAMETERS_SEPARATOR);
    lp->iterator(lp);

//    , double mu, double sigma, unsigned int rank

    pars->mu = (double)atof(lp->next(lp));
    pars->sigma = (double)atof(lp->next(lp));;
    pars->rank = (unsigned int)atol(lp->next(lp));;

    setSeedRandomic(seed);

    data->seed[0] = seed[0];
    data->seed[1] = seed[1];
    data->seed[2] = seed[2];

    data->pars = pars;

    p->data = data;
    p->pick = pickLognormalRandomic;
    p->last = getLastPickedLognormalRandomic;

    lp->dispose(lp);

    return p;
}


TPickedRandomic pickLognormalRandomic(TRandomic *randomic){
//double Lognormal(double a, double b){

/* ====================================================
 * Returns a lognormal distributed positive real number.
 * NOTE: use b > 0.0
 * ====================================================
 */
/* ========================================================================
 * Returns a normal (Gaussian) distributed real number.
 * NOTE: use s > 0.0
 *
 * Uses a very accurate approximation of the normal idf due to Odeh & Evans,
 * J. Applied Statistics, 1974, vol 23, pp 96-97.
 * ========================================================================
 */
  const double p0 = 0.322232431088;     const double q0 = 0.099348462606;
  const double p1 = 1.0;                const double q1 = 0.588581570495;
  const double p2 = 0.342242088547;     const double q2 = 0.531103462366;
  const double p3 = 0.204231210245e-1;  const double q3 = 0.103537752850;
  const double p4 = 0.453642210148e-4;  const double q4 = 0.385607006340e-2;
  double u, t, p, q, z;

  double m=0.0;
  double s=1.0;

  TDataRandomic *pt = (TDataRandomic *)randomic->data;
  TDataLogNormalRandomic *pars = (TDataLogNormalRandomic *)pt->pars;

  u   = erand48(pt->seed);

  if (u < 0.5)
    t = sqrt(-2.0 * log(u));
  else
    t = sqrt(-2.0 * log(1.0 - u));
  p   = p0 + t * (p1 + t * (p2 + t * (p3 + t * p4)));
  q   = q0 + t * (q1 + t * (q2 + t * (q3 + t * q4)));
  if (u < 0.5)
    z = (p / q) - t;
  else
    z = t - (p / q);

  double mu = pars->mu;
  double sigma = pars->sigma;

  pt->pick = (TPickedRandomic)(pars->rank * ( (double)1.0 / ( (double)1.0 + (double) exp(mu + sigma * (m + s * z) ) ) ));

  //pt->pick = (TPickedRandomic)( (double) (exp(mu + sigma * (m + s * z) )) );
  if (pt->pick < 0){
	  printf("eerrpp %d\n", pt->pick);
	  exit(0);
  }

  return pt->pick;

}

TPickedRandomic getLastPickedLognormalRandomic(TRandomic *randomic){
    TDataRandomic *p = (TDataRandomic *)randomic->data;

    return p->pick;
}

TRandomic *createSingletonLognormalRandomic(char *entry){
	static TDictionary *d=NULL;
	TRandomic *p=NULL;
	TKeyDictionary key;
	if (!d)
		d = createDictionary();

	key = d->keyGenesis(entry);
	if (d->has(d,key)){
		p = d->retrieval(d,key);
	}else{
		p = createLognormalRandomic(entry);
		d->insert(d,key,p);
	}

	return p;
}

//Zipf variates
typedef struct _dataZipfRandomic TDataZipfRandomic;
struct _dataZipfRandomic{
    double normC;
    double range;
    double alpha;
};

TPickedRandomic pickZipfRandomic(TRandomic *randomic);
TPickedRandomic getLastPickedZipfRandomic(TRandomic *randomic);

TRandomic *createZipfRandomic(char *entry){
	static unsigned short seed[]={98,76,54};
	double alpha;
	unsigned int range;

	double c = 0;          // Normalization constant
	unsigned int i;

	TRandomic *p = createRandomic();
    TDataRandomic *data = (TDataRandomic *) malloc(sizeof(TDataRandomic));
	TDataZipfRandomic *pars = (TDataZipfRandomic *)malloc(sizeof(TDataZipfRandomic));

    TParameters *lp = createParameters(entry,PARAMETERS_SEPARATOR);
    lp->iterator(lp);

    alpha = (double)atof(lp->next(lp));
    range = (unsigned int)atoi(lp->next(lp));


    setSeedRandomic(seed);
    data->seed[0] = seed[0];
    data->seed[1] = seed[1];
    data->seed[2] = seed[2];



   for (i=1; i<=range; i++)
        c = c + (1.0 / pow((double) i, alpha));
    c = 1.0 / c;


    pars->normC = c;
    pars->alpha = alpha;
    pars->range = range;

    data->pars = pars;

    p->data = data;
    p->pick = pickZipfRandomic;
    p->last = getLastPickedZipfRandomic;

    lp->dispose(lp);

    return p;
}


//===========================================================================
//=  Function to generate Zipf (power law) distributed random variables     =
//=    - Input: alpha and N                                                 =
//=    - Output: Returns with Zipf distributed random variable              =
//===========================================================================
TPickedRandomic pickZipfRandomic(TRandomic *randomic)
{
  TDataRandomic *p = (TDataRandomic *)randomic->data;
  double z;                     // Uniform random number (0 < z < 1)
  double sum_prob;              // Sum of probabilities
  double zipf_value;            // Computed exponential value to be returned
  unsigned int    i;                     // Loop counter

  TDataZipfRandomic *pars = (TDataZipfRandomic *)p->pars;

  // Pull a uniform random number (0 < z < 1)
  do
  {
    z = erand48(p->seed);
    //rand_val(0);
  }while ((z == 0) || (z == 1));

  // Map z to the value
  sum_prob = 0;
  for (i=1; i<=pars->range; i++)
  {
    sum_prob = sum_prob + pars->normC / pow((double) i, pars->alpha);
    if (sum_prob >= z)
    {
      zipf_value = i;
      break;
    }
  }

  // Assert that zipf_value is between 1 and N
  assert((zipf_value >=1) && (zipf_value <= pars->range));

  p->pick = (TPickedRandomic)zipf_value - 1;

  return(p->pick);

}


TPickedRandomic getLastPickedZipfRandomic(TRandomic *randomic){
    TDataRandomic *p = (TDataRandomic *)randomic->data;

    return p->pick;
}

TRandomic *createSingletonZipfRandomic(char *entry){
	static TDictionary *d=NULL;
	TRandomic *p=NULL;
	TKeyDictionary key;
	if (!d)
		d = createDictionary();

	key = d->keyGenesis(entry);
	if (d->has(d,key)){
		p = d->retrieval(d,key);
	}else{
		p = createZipfRandomic(entry);
		d->insert(d,key,p);
	}

	return p;
}

typedef struct _dataFromFileRandomic TDataFromFileRandomic;
struct _dataFromFileRandomic{
    FILE *fp;
    char *source;
};

TPickedRandomic pickFromFileRandomic(TRandomic *randomic);
TPickedRandomic getLastPickedFromFileRandomic(TRandomic *randomic);
void disposeFromFileRandomic(TRandomic *randomic);

TRandomic *createFromFileRandomic(char *entry){
	char *fileName;

    TRandomic *p = createRandomic();
    TDataRandomic *data = (TDataRandomic *) malloc(sizeof(TDataRandomic));
	TDataFromFileRandomic *pars = (TDataFromFileRandomic*)malloc(sizeof(TDataFromFileRandomic));

    TParameters *lp = createParameters(entry,PARAMETERS_SEPARATOR);
    lp->iterator(lp);

    fileName = lp->next(lp);

    pars->source = malloc(strlen(fileName)+1);
    strcpy(pars->source,fileName);

    pars->fp = fopen(fileName, "r");
    if (pars->fp == NULL){
    	printf("PANIC: ERROR FILE NOT FOUND\n");
    	exit(0);
    }

    data->pars = pars;

    p->data = data;
    p->pick = pickFromFileRandomic;
    p->last = getLastPickedFromFileRandomic;
    p->dispose = disposeFromFileRandomic;

    lp->dispose(lp);

    return p;
}

TPickedRandomic pickFromFileRandomic(TRandomic *randomic){
    TDataRandomic *p = (TDataRandomic *)randomic->data;
    TDataFromFileRandomic *pars = (TDataFromFileRandomic *)p->pars;


    if( !feof(pars->fp) )
    	fscanf(pars->fp,"%u",&(p->pick));
    else{
    	printf("ERROR: reached end of file\n");
    	fseek(pars->fp, 0L, SEEK_SET);
    	fscanf(pars->fp,"%u",&(p->pick));
    }

    return p->pick;
}

TPickedRandomic getLastPickedFromFileRandomic(TRandomic *randomic){
    TDataRandomic *p = (TDataRandomic *)randomic->data;

    return p->pick;
}

void disposeFromFileRandomic(TRandomic *randomic){

    if (randomic != NULL){
        TDataRandomic *p = randomic->data;
        if(p){
        	TDataFromFileRandomic *pars = p->pars;
        	if(pars->fp)
        		fclose(pars->fp);
        	if (pars->source)
        		free(pars->source);
        	free(pars);
        	free(p);
        }
        free(randomic);
    }
}


TRandomic *createSingletonFromFileRandomic(char *entry){
	static TDictionary *d=NULL;

	TRandomic *p=NULL;
	TKeyDictionary key;
	if (!d)
		d = createDictionary();

	key = d->keyGenesis(entry);
	if (d->has(d,key)){
		p = d->retrieval(d,key);
	}else{
		p = createFromFileRandomic(entry);
		d->insert(d,key,p);
	}

    return p;
}




//int main(){
//    unsigned int i;
//    unsigned short seed[]={10,1,2};

//     TRandomic *r = initRandomic();
//
//     TPickRandomic *p1 = newPickRandomic(r,"PARETO","3;15;100;");
////     TPickRandomic *p2 = newPickRandomic(r,"EXPONENTIAL","3;12000;");
//     TPickRandomic *p2 = newPickRandomic(r,"LOGNORMAL","8.799;2.349;263121;12000;");
//     for(i=1;i<100;i++)
//    	 printf("%d %d\n", p1->distribution(p1->data), p2->distribution(p2->data));

//////    TPickRandomic *p1 = initPickedPoisson(seed, 10);
//////    TPickRandomic *p2 = initPickedExponential(seed, 10);
////
//////    TPickRandomic *p3 = initPickedThreeTwoRule();
//////    setupPickedThreeTwoRule(p3, 1547296, 52704);
//////
////// 	TPickRandomic *p4 = initPickedZipf(seed,1.4,1600000);
////
//    unsigned int rank=263121;
//
//    TPickRandomic *p = initPickedLognormal(seed, 8.799, 2.349, rank);

//    //printf("%d %d\n", (unsigned int)(k*((double)1.0/(double)getPickedLognormal( p->data )) ), lastPickedLognormal( p->data ));

    //TRandomic *rd = createTruncatedGeometricRandomic("0.4;");
    //TRandomic *video = createZipfRandomic("1.0;1000;");
    //TRandomic *camada = createZipfRandomic("1.0;2;");
    //TRandomic *requisao = createPoissonRandomic("10.8;"); 
    //TRandomic *taxa = createUniformRandomic("4;"); 

//    float taxas[] = {0.1,0.5,1.0,1.5,2.0,2.5,3.0};
//    int k;
    //rd->reset(rd, "10;");
//    int tempo=0;
//    while(tempo < 3600){
//       k = requisao->pick(requisao);     
//       for(i=1;i<k;i++){
//          printf("Segundo %d: %d %d\n", tempo, (unsigned int)video->pick( video ), (unsigned int)camada->pick( camada ));
        //v[k]++;
//       }
//      tempo++;
//    }
//    for(i=1;i<=1000;i++){
//       float taxaL1 = taxas[taxa->pick(taxa)-1];
 //      float taxaL2 = taxaL1 + taxas[taxa->pick(taxa)-1];
//       printf("%d %.2f %.2f\n", i, taxaL1, taxaL2);
//    }		
//    for(i=0;i<=10;i++)
//    	printf("%d %u\n", i, v[i]);

//}



