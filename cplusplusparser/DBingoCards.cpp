
/*****************************************************************************
 *                                                                           *
 *                          BingoCards-Festlegungen                          *
 *                                                                           *
 ***************************   (c) by Hofstetter   ***************************/

/***************************   K O M M E N T A R   ***************************

 *****************************************************************************/



//****************************************************************************

#include "StdAfx.h"
#include "Std.h"
#include "GC.h"
#include "PGC.h"
#include <DBingoCard.h>
#include <DBingoCards.h>
#include <DBets.h>
#include <DSetting.h>
#include <DWins.h>

//****************************************************************************


//****************************************************************************
// global pointer for cards definitions

CDBingoCards *g_pdBingoCards=nullptr;

//----------------------------------------------------------------------------
// protection against multiple objects

uint CDBingoCards::m_unConstructed=0;

//****************************************************************************
//**************** odds ****************
// 91.33% base game without adding extra bonus for Balloon(Bonus1) & GummyBear(Bonus2)! (~0.1%)
// 93.00% each extraball
// + Jackpot: 0.4% FIX or 2% progressiv
//****************************************************************************

static const CDBingoCards::WINTABLE awinTableStandard[] // odds: 91,33% - normal game play without adding extra bonus for Balloon & GummyBear!
  = {
      { 11, CDBingoCard::Corners,          1, 0 },
      { 10, CDBingoCard::OneLine1,         3, 0 },
      { 10, CDBingoCard::OneLine2,         3, 0 },
      { 10, CDBingoCard::OneLine3,         3, 0 },
      {  9, CDBingoCard::SingleV1,         3, 0 },
      {  9, CDBingoCard::SingleV2,         3, 0 },
      {  8, CDBingoCard::Plus,             6, 0 },
      {  7, CDBingoCard::Triangle1,       10, 0 },
      {  7, CDBingoCard::Triangle2,       10, 0 },
      {  6, CDBingoCard::DoublePlus,      30, 0 },
      {  5, CDBingoCard::CandyCane1,      60, 0 },
      {  5, CDBingoCard::CandyCane2,      60, 0 },
      {  4, CDBingoCard::DoubleLine1,    100, 0 },
      {  4, CDBingoCard::DoubleLine2,    100, 0 },
      {  4, CDBingoCard::DoubleLine3,    100, 0 },
      {  3, CDBingoCard::Lollipop,       200, 0 },
      {  2, CDBingoCard::Bonus1,         575, 0 },
      {  1, CDBingoCard::Bonus2,         1150, 0},
      {  0, CDBingoCard::Bingo,          1500, 0},
    };

static const CDBingoCards::WINTABLE awinTableSpainBBar[] 
  = {
      { 11, CDBingoCard::Corners,          1, 0 },
      { 10, CDBingoCard::OneLine1,         3, 0 },
      { 10, CDBingoCard::OneLine2,         3, 0 },
      { 10, CDBingoCard::OneLine3,         3, 0 },
      {  9, CDBingoCard::SingleV1,         3, 0 },
      {  9, CDBingoCard::SingleV2,         3, 0 },
      {  8, CDBingoCard::Plus,             6, 0 },
      {  7, CDBingoCard::Triangle1,       10, 0 },
      {  7, CDBingoCard::Triangle2,       10, 0 },
      {  6, CDBingoCard::DoublePlus,      30, 0 },
      {  5, CDBingoCard::CandyCane1,      60, 0 },
      {  5, CDBingoCard::CandyCane2,      60, 0 },
      {  4, CDBingoCard::DoubleLine1,    100, 0 },
      {  4, CDBingoCard::DoubleLine2,    100, 0 },
      {  4, CDBingoCard::DoubleLine3,    100, 0 },
      {  3, CDBingoCard::Lollipop,       200, 0 },
      {  2, CDBingoCard::Bonus1,         575, 0 },
      {  1, CDBingoCard::Bonus2,         1150, 0},
      {  0, CDBingoCard::Bingo,          1000, 0},
    };


//-- structure for winPattern
typedef struct
{ ushort ausElement[CDBingoCard::NumberOfCardElements];
  uchar ucWin;
} WINCOMBINATION;


static const WINCOMBINATION awinCombination[CDBingoCard::NumberOfWinPattern]
  ={ { { 1,1,1,1,1, 1,1,1,1,1, 1,1,1,1,1}, CDBingoCard::Bingo },
     { { 1,1,1,1,1, 1,0,0,0,1, 1,1,1,1,1}, CDBingoCard::Bonus2 },
     { { 0,1,1,1,0, 1,1,1,1,1, 0,1,1,1,0}, CDBingoCard::Bonus1 },
     { { 1,1,0,1,1, 0,1,1,1,0, 1,1,0,1,1}, CDBingoCard::Lollipop },
     { { 1,1,1,1,1, 1,1,1,1,1, 0,0,0,0,0}, CDBingoCard::DoubleLine3 },
     { { 1,1,1,1,1, 0,0,0,0,0, 1,1,1,1,1}, CDBingoCard::DoubleLine2 },
     { { 0,0,0,0,0, 1,1,1,1,1, 1,1,1,1,1}, CDBingoCard::DoubleLine1 },
     { { 0,1,1,1,1, 0,0,0,0,1, 0,1,1,1,1}, CDBingoCard::CandyCane2 },
     { { 1,1,1,1,0, 1,0,0,0,0, 1,1,1,1,0}, CDBingoCard::CandyCane1 },
     { { 0,1,0,1,0, 1,1,1,1,1, 0,1,0,1,0}, CDBingoCard::DoublePlus },
     { { 0,0,1,0,0, 0,1,0,1,0, 1,1,1,1,1}, CDBingoCard::Triangle2 },
     { { 1,1,1,1,1, 0,1,0,1,0, 0,0,1,0,0}, CDBingoCard::Triangle1 },
     { { 0,0,1,0,0, 1,1,1,1,1, 0,0,1,0,0}, CDBingoCard::Plus },
     { { 0,0,1,0,0, 0,1,0,1,0, 1,0,0,0,1}, CDBingoCard::SingleV2 },
     { { 1,0,0,0,1, 0,1,0,1,0, 0,0,1,0,0}, CDBingoCard::SingleV1 },
     { { 1,1,1,1,1, 0,0,0,0,0, 0,0,0,0,0}, CDBingoCard::OneLine3 },
     { { 0,0,0,0,0, 1,1,1,1,1, 0,0,0,0,0}, CDBingoCard::OneLine2 },
     { { 0,0,0,0,0, 0,0,0,0,0, 1,1,1,1,1}, CDBingoCard::OneLine1 },
     { { 1,0,0,0,1, 0,0,0,0,0, 1,0,0,0,1}, CDBingoCard::Corners },
};

//****************************************************************************



/*****************************************************************************
 Standardkonstruktor.
 *****************************************************************************/

CDBingoCards::CDBingoCards()
{
  //**************************************************************************
  // check multiple objects

  if ( m_unConstructed++ )
    ASSERT(false);

  m_pWinTable=nullptr;
  SetWinTable();

  for (uchar ucPos=0;ucPos<CDBingoCards::MaxNumberOfDrawnNumbers;ucPos++)
    m_aucDrawn[ucPos]=static_cast<uchar>(-1);
  m_ucDrawn=0;
  m_ucService=0;
  m_ucMarkedJokerball=Jokerball;
  //**************************************************************************
}



/*****************************************************************************
 Destructor.
 *****************************************************************************/

CDBingoCards::~CDBingoCards()
{
  m_unConstructed--;
  delete [] m_pWinTable;
}



/*****************************************************************************
 Sets win pattern for configuration
 *****************************************************************************/

void CDBingoCards::SetWinTable(void)
{
  //**************************************************************************

  const WINTABLE *pUseWinTable=awinTableStandard;
  ushort usNumberOfEntries=sizeof(awinTableStandard)/sizeof(WINTABLE);

  if ( g_pdSettings->Use1000MaxFactor() )
  {
    pUseWinTable = awinTableSpainBBar;
    usNumberOfEntries = sizeof(awinTableSpainBBar) / sizeof(WINTABLE);
  }
  delete [] m_pWinTable;
  m_pWinTable=new WINTABLE[usNumberOfEntries];

  m_usNumberOfUsedWinPattern=0;
  for(ushort usPos=0; usPos<usNumberOfEntries; usPos++)
    m_pWinTable[m_usNumberOfUsedWinPattern++]=pUseWinTable[usPos];
}



/*****************************************************************************
 returns the number of used win pattern
 *****************************************************************************/

uchar CDBingoCards::NumberOfWinningPattern(void)
{
  return m_usNumberOfUsedWinPattern;
}



/*****************************************************************************
 returns true when win pattern is used
 *****************************************************************************/

bool CDBingoCards::IsWinPatternUsed(uchar ucWin)
{
  //**************************************************************************
  //check parameter
  if (ucWin<CDBingoCard::NoWin || ucWin>=CDBingoCard::NumberOfWinPattern)
    return false;

  bool bFound=false;
  for (uchar ucPos=0; !bFound && ucPos<CDBingoCard::NumberOfWinPattern; ucPos++ )
  { if (m_pWinTable[ucPos].ucWin==ucWin)
      bFound=true;
  }
  return bFound;
}



/*****************************************************************************
 returns the used position on winplan for a given win
 ucWin ... win
 return ucWinPlanPosition
 *****************************************************************************/

uchar CDBingoCards::GetWinPlanPosition(uchar ucWin)
{
  if (ucWin<CDBingoCard::NoWin || ucWin>=CDBingoCard::NumberOfWinPattern)
    return false;

  uchar ucWinplanPos=CDBingoCard::NumberOfWinPattern;
  bool bFound=false;
  for (uchar ucPos=0; !bFound && ucPos<m_usNumberOfUsedWinPattern; ucPos++ )
  { if (m_pWinTable[ucPos].ucWin==ucWin)
    { ucWinplanPos=m_pWinTable[ucPos].ucWinplanPos;
      bFound=true;
    }
  }
  return ucWinplanPos;
}



/*****************************************************************************
 sets a card with an array of numbers
 ucCard ...... card
 aucNumber ... card numbers
 *****************************************************************************/

void CDBingoCards::SetCard(uchar ucCard, uchar aucNumber[CDBingoCard::NumberOfCardElements])
{
  if (!aucNumber)
  {
    TRACEE("ERROR: nullptr passed instead of array!\n");
    ASSERT(false);
    return;
  }
  m_aCard[ucCard].Set(aucNumber, "Card Id: "+std::to_string(ucCard+1), ucCard);

  //**************************************************************************
}



/*****************************************************************************
 sets a number on a position
 ucDrawn .... drawn position
 ucNumber ... number
 *****************************************************************************/
void CDBingoCards::SetDrawnJokerball(uchar ucDrawn, uchar ucNumber)
{
  if (m_aucDrawn[ucDrawn]!=CDBingoCards::Jokerball)
  { ASSERT(false);
    return;
  }
  m_aucDrawn[ucDrawn]=ucNumber;
}



/*****************************************************************************
 sets the drawn numbers and marks numbers on cards
 aucDrawn ... array of drawn numbers
 *****************************************************************************/

void CDBingoCards::SetDrawnNumbers(uchar aucDrawn[CDBingoCards::MaxNumberOfDrawnNumbers], uchar ucDrawn, bool bEvaluate)
{
  if (!aucDrawn)
  {
    TRACEE("ERROR: nullptr passed instead of array!\n");
    ASSERT(false);
    return;
  }
  for (uchar ucPos = 0; ucPos<ucDrawn; ucPos++)
  {
    if (aucDrawn[ucPos] != CDBingoCards::Jokerball)
    {
      if (m_aCard[GetCardForNumber(aucDrawn[ucPos])].GetPosforNumber(aucDrawn[ucPos])<CDBingoCard::NumberOfCardElements)
        m_aCard[GetCardForNumber(aucDrawn[ucPos])].SetElementState(aucDrawn[ucPos], CDBingoCard::ElementHit, ucPos);
    }
    m_aucDrawn[ucPos] = aucDrawn[ucPos];
  }
  // set undrawn numbers to -1
  for (uchar ucPos = ucDrawn; ucPos<CDBingoCards::MaxNumberOfDrawnNumbers; ucPos++)
    m_aucDrawn[ucPos]=static_cast<uchar>(-1);;
  m_ucDrawn = ucDrawn;

  ulong aulExpectedWins[CDBingoCard::NumberOfCardElements];
  ulong ulWinPattern;
  for (uchar ucCard = 0; ucCard<CDBingoCards::NumberOfCards; ucCard++)
  { //set expected
    if (bEvaluate)
      m_aCard[ucCard].GetExpectedElements(aulExpectedWins);
    else
      CardGetExpectedElements(ucCard, aulExpectedWins);
    CardSetExpectedElements(ucCard, aulExpectedWins);

    //set wins
    if (bEvaluate)
      m_aCard[ucCard].EvaluateWin(ulWinPattern);
    else
      CardGetCurrentWinPattern(ucCard, ulWinPattern);
    CardSetCurrentWinPattern(ucCard, ulWinPattern);
    CardMarkWin(ucCard, ulWinPattern);
  }
  //--------------------------------------------------------------
}



/*****************************************************************************
 Set card bet
 *****************************************************************************/

void CDBingoCards::SetCardsBet(uchar ucCard, CGameValue gvBet)
{
  if (ucCard>=CDBingoCards::NumberOfCards)
  {
    TRACEE("ERROR: SetCardsBet() card %d not found\n", ucCard);
    ASSERT(false);
    return;
  }
  m_aCard[ucCard].SetBet(gvBet);
}



/*****************************************************************************
 returns total bet from all open cards
 *****************************************************************************/
CGameValue CDBingoCards::GetTotalCardsBet(void)
{
  CGameValue gvTotalCardBet=0;
  for (uchar ucPos=0; ucPos<CDBingoCards::NumberOfCards; ucPos++)
    gvTotalCardBet+=m_aCard[ucPos].GetBet();
  return gvTotalCardBet;
}



/*****************************************************************************
 resets the card (expected wins...)
 ucCard ... card (if card == NumberofCards all cards are reset)
 *****************************************************************************/

void CDBingoCards::ResetCard(uchar ucCard)
{
  if (ucCard>=CDBingoCards::NumberOfCards)
  { for (uchar ucPos=0; ucPos<CDBingoCards::NumberOfCards; ucPos++)
      m_aCard[ucPos].Reset();
  }
  else
    m_aCard[ucCard].Reset();
}



/*****************************************************************************
 returns card for given number
 ucNumber .. number
 returns card as uchar
 *****************************************************************************/

uchar CDBingoCards::GetCardForNumber(uchar ucNumber)
{
  bool bFound=false;
  uchar ucCard=0;
  while (!bFound && ucCard<CDBingoCards::NumberOfCards)
  { if ( m_aCard[ucCard].NumberExists(ucNumber) )
      bFound=true;
    else
      ucCard++;
  }
  if (!bFound)
  { ASSERT(false);
    return 0;
  }
  return ucCard;
}



/*****************************************************************************
 sets expected wins for card
 ucCard ... card
 aucWin ... wins array for each position on card
 *****************************************************************************/

void CDBingoCards::CardSetExpectedElements(uchar ucCard, ulong aulWin[CDBingoCard::NumberOfCardElements])
{
  if (ucCard>=CDBingoCards::NumberOfCards)
  {
    TRACEE("ERROR: CardSetExpectedElements() card %d not found\n", ucCard);
    ASSERT(false);
    return;
  }
  if (m_aCard[ucCard].GetState()!=CDBingoCard::Active)
    return;

  m_aCard[ucCard].SetExpectedWins(aulWin);
}



/*****************************************************************************
 gets expected wins for card
 ucCard ... card
 aucWin ... wins array for each position on card
 *****************************************************************************/

void CDBingoCards::CardGetExpectedElements(uchar ucCard, ulong aulWin[CDBingoCard::NumberOfCardElements])
{
  if (!aulWin)
  {
    TRACEE("ERROR: nullptr passed instead of array!\n");
    ASSERT(false);
    return;
  }
  if (ucCard>=CDBingoCards::NumberOfCards)
  {
    TRACEE("ERROR: CardGetExpectedElements() card %d not found\n", ucCard);
    ASSERT(false);
    return;
  }
  for (uchar ucPos=0;ucPos<CDBingoCard::NumberOfCardElements;ucPos++)
    aulWin[ucPos]=m_aCard[ucCard].GetExpectedWin(ucPos);
}



/*****************************************************************************
 sets possible win pattern for given card
 ucCard ... card
 aucWinPattern ... array of possible wins
 *****************************************************************************/

void CDBingoCards::CardSetCurrentWinPattern(uchar ucCard, ulong ulWinPattern)
{
  if (ucCard>=CDBingoCards::NumberOfCards)
  {
    TRACEE("ERROR: CardSetCurrentWinPattern() card %d not found\n", ucCard);
    ASSERT(false);
    return;
  }
  m_aCard[ucCard].SetCurrentWins(ulWinPattern);
}



/*****************************************************************************
 gets current win pattern for given card
 ucCard ... card
 aucWinPattern ... array of possible wins
 *****************************************************************************/

void CDBingoCards::CardGetCurrentWinPattern(uchar ucCard, ulong &ulWinPattern)
{
  if ( ucCard>=CDBingoCards::NumberOfCards )
  {
    TRACEE("ERROR: CardGetCurrentWinPattern() card %d not found\n", ucCard);
    ASSERT(false);
    ulWinPattern=0;
    return;
  }
  ulWinPattern=m_aCard[ucCard].GetCurrentWins();
}



/*****************************************************************************
 checks current win pattern with actual pattern and returns showpattern
 *****************************************************************************/

bool CDBingoCards::CardGetNewWinPattern(uchar ucCard, ulong ulCurrentWinPattern,
                                        SHOWPATTERN &showPattern)
{
  if ( ucCard>=CDBingoCards::NumberOfCards )
  {
    TRACEE("ERROR: CardGetNewWinPattern() card %d not found\n", ucCard);
    ASSERT(false);
    return false;
  }

  ulong ulOldWinPattern;
  CardGetCurrentWinPattern(ucCard, ulOldWinPattern);

  showPattern.ucCard=CDBingoCards::NumberOfCards;
  showPattern.ulSoundLength=0;

  //remove all old pattern which still are in new pattern
  showPattern.ulWinPattern=ulCurrentWinPattern;
  for (uchar ucWin = CDBingoCard::FirstGameWin; ucWin < CDBingoCard::NumberOfWinPattern; ucWin++)
  {
    if ((ulOldWinPattern&(1 << ucWin)) && (ulCurrentWinPattern&(1 << ucWin)))
      showPattern.ulWinPattern -= 1 << ucWin;
  }

  if (showPattern.ulWinPattern)
  {
    showPattern.ucCard=ucCard;
    return true;
  }
  return false;
}



/*****************************************************************************
 sets current card status
 ucCard ... card
 ucCardState ... card status
 *****************************************************************************/

void CDBingoCards::SetCardState(uchar ucCard, uchar ucStatus)
{
  if ( ucCard>=CDBingoCards::NumberOfCards)
  {
    TRACEE("ERROR: SetCardState() card %d not found\n", ucCard);
    ASSERT(false);
    return;
  }
  m_aCard[ucCard].SetState(ucStatus);
  //**************************************************************************
}



/*****************************************************************************
 returns current card status
 ucCard ... card
 *****************************************************************************/

uchar CDBingoCards::GetCardState(uchar ucCard)
{
  if (ucCard>=CDBingoCards::NumberOfCards)
  {
    TRACEE("ERROR: GetCardState() card %d not found\n", ucCard);
    ASSERT(false);
    return 0;
  }
  return m_aCard[ucCard].GetState();
  //**************************************************************************
}



/*****************************************************************************
 returns true if a bingo is possible for this card
 ucCard ... card
 *****************************************************************************/
bool CDBingoCards::IsBingoWithIn30Possible(uchar ucCard)
{
  if (ucCard>=CDBingoCards::NumberOfCards)
  {
    TRACEE("ERROR: IsBingoWithIn30Possible() card %d not found\n", ucCard);
    ASSERT(false);
    return false;
  }

  if (IsInactiveCards())
    return false;

  return GetCard(ucCard).IsBingoWithIn30Possible();
}


/*****************************************************************************
checks if there is any inactive card
 *****************************************************************************/

bool CDBingoCards::IsInactiveCards()
{
  for (uchar ucCard=0;ucCard<CDBingoCards::NumberOfCards;ucCard++)
    if (GetCardState(ucCard)==CDBingoCard::Inactive)
      return true;
 return false;
}



/*****************************************************************************
 marks a number with the given marktype
 ucNumber ..... number to mark
 ucMarkType ... marktype
 *****************************************************************************/

void CDBingoCards::MarkNumber(uchar ucNumber, uchar ucMarkType, uchar ucDrawnPos)
{
  bool bFound = false;
  for (uchar ucCard = 0; !bFound && ucCard<CDBingoCards::NumberOfCards; ucCard++)
  {
    if (m_aCard[ucCard].NumberExists(ucNumber))
    {
      m_aCard[ucCard].SetElementState(ucNumber, ucMarkType, ucDrawnPos);
      bFound = true;
    }
  }
  //**************************************************************************
}



/*****************************************************************************
 marks a win on a card
 ucCard ....... card
 ulWinPattern . pattern to mark win
 *****************************************************************************/

void CDBingoCards::CardMarkWin(uchar ucCard, ulong ulWinPattern)
{
  if (ucCard>=CDBingoCards::NumberOfCards )
  {
    TRACEE("ERROR: CardMarkWin() card %d not found\n", ucCard);
    ASSERT(false);
    return;
  }

  ulong ulCurrWinPattern;
  for (uchar ucWin=CDBingoCard::FirstGameWin; ucWin<CDBingoCard::NumberOfWinPattern; ucWin++)
  { if ( ulWinPattern&(1<<ucWin) )
    { CDBingoCards::GetWinPattern(ucWin, ulCurrWinPattern);
      for (uchar ucPos=0;ucPos<CDBingoCard::NumberOfCardElements;ucPos++)
      { if ( (ulCurrWinPattern & (1UL<<ucPos))==(1UL<<ucPos) )
          m_aCard[ucCard].SetElementState(m_aCard[ucCard].GetNumberforPos(ucPos), CDBingoCard::ElementWin, CDBingoCards::MaxNumberOfDrawnNumbers);
      }
    }
  }

  //**************************************************************************
}



/*****************************************************************************
 returns the winpattern for a given win
 ucWin ........ win
 ulPattern ... win pattern shifted
 returns true when winpattern is found
 *****************************************************************************/

bool CDBingoCards::GetWinPattern(uchar ucWin, ulong &ulPattern)
{
  if (ucWin<CDBingoCard::NoWin || ucWin>=CDBingoCard::NumberOfWinPattern)
  { ASSERT(false);
    return false;
  }
  ulPattern=0;
  bool bFound=false;
  for (uchar ucPos=0;!bFound &&  ucPos<CDBingoCard::NumberOfWinPattern; ucPos++)
    if (awinCombination[ucPos].ucWin==ucWin )
    { ulPattern=CalculateCombinationNumber(awinCombination[ucPos].ausElement);
      bFound=true;
    }
  return bFound;
  //**************************************************************************
}



/*****************************************************************************
 calculates combination number for a given array
 ausCombination ... array of win combinations
 returns combination as ulong
 *****************************************************************************/

ulong CDBingoCards::CalculateCombinationNumber(const ushort ausCombination[CDBingoCard::NumberOfCardElements])
{
  if (!ausCombination)
  {
    TRACEE("ERROR: nullptr passed instead of array!\n");
    ASSERT(false);
    return 0;
  }
  ulong ulCombination=0;
  for (uchar ucPos=0; ucPos<CDBingoCard::NumberOfCardElements; ucPos++)
    ulCombination+=ausCombination[ucPos]<<ucPos;
  return ulCombination;
  //**************************************************************************
}



/*****************************************************************************
 returns the factor for a given win
 gvBet ... bet
 ucWin ... win
 returns ulong factor
 *****************************************************************************/

ulong CDBingoCards::GetFactor(uchar ucCard, const CGameValue& gvBet, uchar ucWin, bool bBingoIn30, uchar ucNextExtraball)
{
  UNREFERENCED_PARAMETER(ucCard);
  UNREFERENCED_PARAMETER(gvBet);
  UNREFERENCED_PARAMETER(ucNextExtraball);

  //**************************************************************************
  // Parameter berprfen

  if (ucWin==CDBingoCard::NoWin
      || ucWin>=CDBingoCard::NumberOfWinPattern)
  { ASSERT(false);
    return 0;
  }

  if (ucWin == CDBingoCard::Bingo)
  {
    if (bBingoIn30)
      return GetBingoIn30Factor(g_pdSettings->UseJackpot());
  }

  ulong ulFactor=0;
  bool bFound=false;
  for (uchar ucPos=0; !bFound && ucPos<CDBingoCard::NumberOfWinPattern; ucPos++)
  { if ( m_pWinTable[ucPos].ucWin==ucWin )
    { ulFactor=m_pWinTable[ucPos].ulFactor;
      bFound=true;
    }
  }
  return ulFactor;
  //**************************************************************************
}



/*****************************************************************************
 returns the bingo in 30 factor
 *****************************************************************************/

ulong CDBingoCards::GetBingoIn30Factor(bool bJackpot)
{
  ushort usConfiguration = CConfiguration::Get();
  if (bJackpot)
  {
    return 1500;
  }

  switch (usConfiguration)
  {
    case CConfiguration::SpainB:
      if ( !SysSettingGetOption(SysSetting::OptionIsSpainBSalon) )
        return 1000;    // limit wins for bar
      break;
    default: break;
  }

  return 3000; // 3000= ~0,4%       // 8000= ~2% Jackpot value (for base game bet)
}



uchar CDBingoCards::GetDrawnNumbers(uchar aucDrawn[CDBingoCards::MaxNumberOfDrawnNumbers])
{
  if (!aucDrawn)
  {
    TRACEE("ERROR: nullptr passed instead of array!\n");
    ASSERT(false);
    return 0;
  }
  for(uchar ucPos=0; ucPos<m_ucDrawn; ucPos++)
    aucDrawn[ucPos]=m_aucDrawn[ucPos];
  return m_ucDrawn;
}



/*****************************************************************************
 set/get for service ( necessary to show correct bingo/jackpot win)
*****************************************************************************/

void CDBingoCards::SetService(uchar ucService)
{
  m_ucService=ucService;
}

bool CDBingoCards::IsService(void)
{
  return m_ucService>0;
}



/*****************************************************************************
 set/get the marked jokerball
 only necessary if no touchscreen is available to step through possible Jokerballs
 *****************************************************************************/

void CDBingoCards::SetMarkedJokerball(uchar ucMarkedJokerball)
{
  m_ucMarkedJokerball=ucMarkedJokerball;
}

uchar CDBingoCards::GetMarkedJokerball()
{
  return m_ucMarkedJokerball;
}



/*****************************************************************************
 checks if pattern1 exists in patter2
 ulPatternExists ... pattern to test
 ulPatternSearch ... pattern which may includes existing pattern
 return true for found pattern
 *****************************************************************************/

bool CDBingoCards::PatternExistsIn(ulong ulPatternExists, ulong ulPatternSearch)
{
  //if both are equal
  if (ulPatternExists == ulPatternSearch)
    return true;

  if ( (ulPatternExists != ulPatternSearch)
        && (ulPatternExists & ulPatternSearch) == ulPatternExists)
    return true;
  return false;
}



/*****************************************************************************
 calculates the expected win value for given drawn numbers
 dJokerProbability ... probability for a Joker
 returns the expected win value
 *****************************************************************************/

ulong CDBingoCards::GetExpectedWinValue()
{
  ulong ulCardPattern[CDBingoCards::NumberOfCards];
  for (uchar ucCard=0;ucCard<CDBingoCards::NumberOfCards;ucCard++)
    ulCardPattern[ucCard]=m_aCard[ucCard].GetPattern(m_ucDrawn, m_aucDrawn);

  ulong ulMaxWin=0, ulCurrentWin=0;
  ulong ulAllMaxWin=0, ulAllCurrentWin=0;
  for (uchar ucCard=0;ucCard<CDBingoCards::NumberOfCards;ucCard++)
  { if (m_aCard[ucCard].GetState()==CDBingoCard::Active)
    { m_aCard[ucCard].GetExpectedWinsforPattern(ulMaxWin,ulCurrentWin, ulCardPattern[ucCard]);
      if (ulAllMaxWin<ulMaxWin)
        ulAllMaxWin=ulMaxWin;
      ulAllCurrentWin+=ulCurrentWin;
    }
  }

  return ulAllCurrentWin;
}



/*****************************************************************************
 calculates extraballs betFactor and jokerball
 ulBetFactor ... returned betfactor
 bJokerball .... true if jokerball won
 *****************************************************************************/

ulong CDBingoCards::CalculateExtraball(ushort usOdds, ulong &ulBetFactor, bool &bJokerball)
{
  //get game odds
  ulong ulFactor=GetExpectedWinValue();

  double dExtraBallValue = (double)ulFactor / (NumberOfAllCardNumbers - m_ucDrawn);

  ulBetFactor=(ulong)ceil(dExtraBallValue/((double)usOdds/1000));

  ulong ulEBChance=0;
  if ( ulBetFactor )
  {
   double dBetFactor=(double)ulBetFactor*usOdds/1000;
   ulEBChance=10000*(1-(dExtraBallValue /dBetFactor));
  }
  ulong ulTestChance=g_pPGNetRandom->GetReal(10000);

  if ( ulFactor<20 || ulTestChance<ulEBChance )
    ulBetFactor=0;

  bJokerball=false;

  return ulBetFactor;
}



/*****************************************************************************
 checks drawn numbers if an extra ball can be offered
 returns true if extra ball is offered
 *****************************************************************************/

bool CDBingoCards::ExtraballOffered(void)
{
  return g_pdWins->IsExtraPossible(m_ucDrawn);
}



/*****************************************************************************
 returns the odds for extra balls depending on used win table in base game
 *****************************************************************************/

ushort CDBingoCards::GetExtraBallOdds(void)
{
  return 930;
}



/*****************************************************************************
 checks accumulated bet and current win if Extraball is possible
 gvCardBet ... card bet
 gvAccumulatedBet ... accumulated bet (gamebet plus extra ball bets)
 returns true if extra ball is offered
 *****************************************************************************/

bool CDBingoCards::IsWithinBetLimitation(CGameValue gvCardBet, CGameValue gvAccumulatedBet)
{
  bool bLimit=true;
  ulong ulCardPattern[CDBingoCards::NumberOfCards];
  for (uchar ucCard=0;ucCard<CDBingoCards::NumberOfCards;ucCard++)
    ulCardPattern[ucCard]=GetCard(ucCard).GetPattern(m_ucDrawn, m_aucDrawn);

  CWinValue wvWin=CWinValue::Zero();
  for (uchar ucCard=0;ucCard<CDBingoCards::NumberOfCards;ucCard++)
  { ulong ulWinPattern;
    if ( GetCard(ucCard).EvaluateWinForPattern(ulWinPattern, ulCardPattern[ucCard]) )
    { for (uchar ucWin=CDBingoCard::FirstGameWin; ucWin<CDBingoCard::NumberOfWinPattern; ucWin++)
      { if ( ulWinPattern & (1<<ucWin) )
          wvWin+=g_pdWins->GetWinEx(gvCardBet, ucWin, IsBingoWithIn30Possible(ucCard), ucCard);
      }
    }
  }// card

  //check if wins are enough to pay next bingo ball (accumulatedBet-win<TotalBet)
  if ( g_pdSettings->UseAccumulatedBetWithWinsForInsufficientTotalBet() )
  { CGameValue gvTotal=g_pdBets->GetMaxTotalExtraBet();
    gvTotal+=wvWin.GetTotalValue();
    if ( gvAccumulatedBet>gvTotal )
       bLimit=false;
  }
  return bLimit;
}



/*****************************************************************************
 Returns a reference to a reel definitions
 ucReel ... reel number
 return ... reference
 *****************************************************************************/

const CDBingoCard &CDBingoCards::GetCard(uchar ucCard) const
{
  if ( ucCard>=NumberOfCards )
    { ASSERT(false);
      return m_aCard[0];
    }
  return m_aCard[ucCard];
}



/*****************************************************************************
 Dumps card information
 *****************************************************************************/

void CDBingoCards::DumpCards(void)
{
#ifdef _DEBUG
  TRACEDUMP("\n-------------------------------------------------------------\n");
  for (uchar ucCard=0;ucCard<CDBingoCards::NumberOfCards;ucCard++)
    m_aCard[ucCard].Dump();
#endif // _DEBUG
}

//****************************************************************************
