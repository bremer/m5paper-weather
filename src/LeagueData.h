#include <WString.h>

class LeagueData {
public:
   String leagueNextTeam1;  // OpenLiga
   String leagueNextTeam2;  
   String leagueNextTime;  
   String leagueSpieltag;  

public:
   LeagueData()
      : leagueNextTeam1("")
      , leagueNextTeam2("")
      , leagueNextTime("")
      , leagueSpieltag("")
   {
   }
};
