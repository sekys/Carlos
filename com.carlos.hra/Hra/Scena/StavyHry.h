#pragma once

/// Enum v ktorom su zoskupene vsetky mozne stavy hry 
enum StavyHry {
	UVODNA_OBRAZOVKA,  /**< Stav v ktorom je hra po zapnuti*/
	HRAJE_HRU, /**< Stav do ktoreho sa hra prepne ked sa dotkne displaya - ked hra hru */
	OBRAZOVKA_PREHRAL, /**< Stav do ktoreho sa hra dostane ked vyleti s lietadlom mimo dovoleneho uzemia */
	OBRAZOVKA_SKORE, /**< Stav v ktorom je hra ked sa zobrazuje skore */
	TOURIST_INFO
};
