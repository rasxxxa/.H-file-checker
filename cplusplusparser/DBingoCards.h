/*****************************************************************************
 *                                                                           *
 *                                  BingoCards                               *
 *                                                                           *
 ***************************   (c) by Hofstetter   ***************************/

/***************************   K O M M E N T A R   ***************************

 class for bingocards

 *****************************************************************************/


#pragma once

//****************************************************************************
// class bingocards


//-- structure for detailed win information
typedef struct
{ uchar ucCard;
  uchar ucDrawnNumber;
  ulong ulWinPattern;
  ulong ulSoundLength;
  bool bFlash;
} SHOWPATTERN;


//****************************************************************************
// Klasse BingoKarten

class CDBingoCards
{
public:
  //-- Number of cards.
  enum { NumberOfCards=4 }; // random comment
  enum { NumberOfCardNumbers=90 };
  enum { Jokerball=NumberOfCardNumbers };
  enum { NumberOfAllCardNumbers=NumberOfCards*CDBingoCard::NumberOfCardElements };

  //-- number of numbers to draw.
  enum { NumberOfDrawnBingoIn30=30, NumberOfDrawnNumbers=30 };
  enum { NumberOfExtraDrawnNumbers=11 };
  enum { MaxNumberOfDrawnNumbers=NumberOfDrawnNumbers+NumberOfExtraDrawnNumbers };

  //-- Constructor: Creates the cards.
  CDBingoCards();

  //-- Destructor.
  ~CDBingoCards();

  //-- sets win table for configuration
  void SetWinTable(void);

  //-- returns true when win pattern is used in set configuration
  bool IsWinPatternUsed(uchar ucWin);

  //-- returns number of winpattern
  uchar NumberOfWinningPattern(void);

  //-- returns winplan position for given win
  uchar GetWinPlanPosition(uchar ucWin);

  //-- sets a card with given numbers
  void SetCard(uchar ucCard, uchar aucNumber[CDBingoCard::NumberOfCardElements]);

  //-- resets card  (remove element stati)
  void ResetCard(uchar ucCard);

  //-- set/get card state for card
  void SetCardState(uchar ucCard, uchar ucState);
  uchar GetCardState(uchar ucCard);

  //-- checks if bingo in 30 is possible for given card
  bool IsBingoWithIn30Possible(uchar ucCard);

  //-- checks if there is any inactive card
  bool IsInactiveCards();

  //-- set drawn numbers
  void SetDrawnNumbers(uchar aucDrawn[CDBingoCards::MaxNumberOfDrawnNumbers], uchar ucDrawn, bool bEvaluate);

  //-- set the jokerball
  void SetDrawnJokerball(uchar ucDrawn, uchar ucNumber);

  //-- sets current card bet
  void SetCardsBet(uchar ucCard, CGameValue gvBet);

  //-- returns total cards bet
  CGameValue GetTotalCardsBet(void);

  //-- returns card for given number
  uchar GetCardForNumber(uchar ucNumber);

  //-- sets expected elements
  void CardSetExpectedElements(uchar ucCard, ulong aulWin[CDBingoCard::NumberOfCardElements]);
  void CardGetExpectedElements(uchar ucCard, ulong aulWin[CDBingoCard::NumberOfCardElements]);

  //-- returns an expected win value for already drawn numbers
  ulong GetExpectedWinValue();

  //-- sets win pattern for card
  void CardSetCurrentWinPattern(uchar ucCard, ulong ulWinPattern);
  void CardGetCurrentWinPattern(uchar ucCard, ulong &ulWinPattern);

  //-- returns new winpattern for given card
  bool CardGetNewWinPattern(uchar ucCard, ulong ulCurrentWinPattern, SHOWPATTERN &showPattern);

  //-- marks win on card
  void CardMarkWin(uchar ucCard, ulong ulWinPattern);

  //-- marks a given number
  void MarkNumber(uchar ucNumber, uchar ucMarkType, uchar ucDrawnPos);

  //-- calulates extra ball values (bet factor, jokerball)
  ulong CalculateExtraball(ushort usOdds, ulong &ulBetFactor, bool &bJokerball);

  //-- returns true when extra ball is offerd
  bool ExtraballOffered(void);

  //-- returns the odds to use for extra balls
  ushort GetExtraBallOdds(void);

  //-- returns true when bet limitation is not reached
  bool IsWithinBetLimitation(CGameValue gvCardBet, CGameValue gvAccumulatedBet);

  //-- returns the factor for a given win
  ulong GetFactor(uchar ucCard, const CGameValue& gvBet, uchar ucWin, bool bBingoIn30 = false, uchar ucNextExtraball = 0);

  //-- returns the bingo in 30 factor
  ulong GetBingoIn30Factor(bool bJackpot);

  uchar GetDrawnNumbers(uchar aucDrawn[CDBingoCards::MaxNumberOfDrawnNumbers]);

  //-- set/get for service ( necessary to show correct bingo/jackpot win)
  void SetService(uchar ucService);
  bool IsService(void);

  //-- set/get the marked jokerball (only necessary if no touchscreen is available to step through possible Jokerballs)
  void SetMarkedJokerball(uchar ucMarkedJokerball);
  uchar GetMarkedJokerball();

  //-- Returns a reference to a card definition.
  //   ucCard ... card number
  //   return ... reference
  const CDBingoCard &GetCard(uchar ucCard) const;

  //-- Dumps information for a reel assignment.
  void DumpCards(void);

  //-- returns a winpattern structure for the given win
  static bool GetWinPattern(uchar ucWin, ulong &ulPattern);

  //-- checks if patternExists is included in patternSearch
  static bool PatternExistsIn(ulong ulPatternExists, ulong ulPatternSearch);

  //-- wintable - win/factor/type and position on winplan
  typedef struct
  { uchar ucWinplanPos;
    uchar ucWin;
    ulong ulFactor;
    uchar ucWinType;
  } WINTABLE;

private:
  static uint m_unConstructed;
  CDBingoCard m_aCard[NumberOfCards];
  uchar m_aucDrawn[CDBingoCards::MaxNumberOfDrawnNumbers];
  uchar m_ucDrawn;
  uchar m_ucService;

  WINTABLE *m_pWinTable;
  ushort m_usNumberOfUsedWinPattern;
  uchar m_ucMarkedJokerball;

  static ulong CalculateCombinationNumber(const ushort ausCombination[CDBingoCard::NumberOfCardElements]);

  // prevent copy constructor and assignment operator
  CDBingoCards(const CDBingoCards &);
  CDBingoCards &operator=(const CDBingoCards &);
};


//----------------------------------------------------------------------------
//! \var global pointer for bingocards

extern CDBingoCards *g_pdBingoCards;



//****************************************************************************
