#ifndef DONT_USE_STB
    #include <mod/thirdparty/stb_sprintf.h>
    #define sprintf stbsp_sprintf
    #define snprintf stbsp_snprintf
#endif
#include "config.h"
#include <mod/logger.h>

#include "amlmod.h"
#include "iaml.h"
#if !defined(__AML) && defined(_ICFG)
	ICFG* icfg;
#else
	#include <fstream>
	#include "thirdparty/inipp.h"
#endif
#ifdef __AML
	extern char g_szCfgPath[0xFF];
#endif

inline bool str_equal(const char* str1, const char* str2) { 
    for ( ; *str1 == *str2 && *str1 != 0; ++str1, ++str2 ); 
        return *str2 == *str1; 
}

extern ModInfo* modinfo;
ConfigEntry* Config::pLastEntry = NULL;

Config::Config(const char* szName)
{
#if !defined(__AML) && defined(_ICFG)
	m_pICFG = (ICFG*)GetInterface("AMLConfig");
	m_iniMyConfig = m_pICFG->InitIniPointer();
#else
	m_iniMyConfig = new inipp::Ini<char>();
    logger->Info("inipp 0x%16X, 0x%16X", m_iniMyConfig, *(void**)m_iniMyConfig);
#endif
	m_bInitialized = false;
    m_bValueChanged = false;
    m_szName = szName;

	#ifndef __AML
		Init();
	#endif
}

void Config::Init()
{
	if(m_bInitialized) return;
	m_bInitialized = true;
    
	#if !defined(__AML) && defined(_ICFG)
		m_pICFG->ParseInputStream(m_iniMyConfig, m_szName);
	#else
		char path[0xFF];
		#ifdef __AML
    		snprintf(path, sizeof(path), "%s/%s.ini", g_szCfgPath, m_szName);
			std::ifstream cfgStream(path);
		#else
    		snprintf(path, sizeof(path), "%s/%s.ini", aml->GetConfigPath(), m_szName);
			std::ifstream cfgStream(path);
		#endif
		if(cfgStream.is_open())
		{
			((inipp::Ini<char>*)m_iniMyConfig)->parse(cfgStream);
		}
		cfgStream.close();
	#endif
}

void Config::Save()
{
	if(!m_bInitialized || !m_bValueChanged) return;
    
    m_bValueChanged = false;
	#if !defined(__AML) && defined(_ICFG)
		m_pICFG->GenerateToOutputStream(m_iniMyConfig, m_szName);
	#else
		char path[0xFF];
		#ifdef __AML
    		snprintf(path, sizeof(path), "%s/%s.ini", g_szCfgPath, m_szName);
			std::ofstream cfgStream(path);
		#else
    		snprintf(path, sizeof(path), "%s/%s.ini", aml->GetConfigPath(), m_szName);
			std::ofstream cfgStream(path);
		#endif
		if(cfgStream.is_open())
		{
			((inipp::Ini<char>*)m_iniMyConfig)->generate(cfgStream);
		}
		cfgStream << "";
		cfgStream.close();
	#endif
}

ConfigEntry* Config::Bind(const char* szKey, const char* szDefaultValue, const char* szSection)
{
	if(!m_bInitialized) return NULL;
	ConfigEntry* pRet = new ConfigEntry;
	pRet->m_pBoundCfg = this;
    strncpy(pRet->m_szMySection, szSection, sizeof(pRet->m_szMySection));
    strncpy(pRet->m_szMyKey, szKey, sizeof(pRet->m_szMyKey));
    strncpy(pRet->m_szDefaultValue, szDefaultValue, sizeof(pRet->m_szDefaultValue));
	const char* tryToGetValue;
	#if !defined(__AML) && defined(_ICFG)
		tryToGetValue = m_pICFG->GetValueFrom(m_iniMyConfig, szSection, szKey);
	#else
		tryToGetValue = ((inipp::Ini<char>*)m_iniMyConfig)->sections[szSection][szKey].c_str();
	#endif
	if(tryToGetValue[0] == '\0')
		pRet->SetString(szDefaultValue);
	else
    {
        bool bShouldChange = !pRet->m_pBoundCfg->m_bValueChanged;
		pRet->SetString(tryToGetValue);
        if(bShouldChange) pRet->m_pBoundCfg->m_bValueChanged = false;
    }
	Save();
    pLastEntry = pRet;
	return pRet;
}

ConfigEntry* Config::Bind(const char* szKey, int nDefaultValue, const char* szSection)
{
	if(!m_bInitialized) return NULL;
	ConfigEntry* pRet = new ConfigEntry;
	pRet->m_pBoundCfg = this;
    strncpy(pRet->m_szMySection, szSection, sizeof(pRet->m_szMySection));
    strncpy(pRet->m_szMyKey, szKey, sizeof(pRet->m_szMyKey));
    snprintf(pRet->m_szDefaultValue, sizeof(pRet->m_szDefaultValue), "%d", nDefaultValue);
	const char* tryToGetValue;
	#if !defined(__AML) && defined(_ICFG)
		tryToGetValue = m_pICFG->GetValueFrom(m_iniMyConfig, szSection, szKey);
	#else
		tryToGetValue = ((inipp::Ini<char>*)m_iniMyConfig)->sections[szSection][szKey].c_str();
	#endif
	if(tryToGetValue[0] == '\0')
		pRet->SetInt(nDefaultValue);
    else
    {
        bool bShouldChange = !pRet->m_pBoundCfg->m_bValueChanged;
        pRet->SetString(tryToGetValue);
        if(bShouldChange) pRet->m_pBoundCfg->m_bValueChanged = false;
    }
	Save();
    pLastEntry = pRet;
	return pRet;
}

ConfigEntry* Config::Bind(const char* szKey, float flDefaultValue, const char* szSection)
{
	if(!m_bInitialized) return NULL;
	ConfigEntry* pRet = new ConfigEntry;
	pRet->m_pBoundCfg = this;
    strncpy(pRet->m_szMySection, szSection, sizeof(pRet->m_szMySection));
    strncpy(pRet->m_szMyKey, szKey, sizeof(pRet->m_szMyKey));
    snprintf(pRet->m_szDefaultValue, sizeof(pRet->m_szDefaultValue), "%f", flDefaultValue);
	const char* tryToGetValue;
	#if !defined(__AML) && defined(_ICFG)
		tryToGetValue = m_pICFG->GetValueFrom(m_iniMyConfig, szSection, szKey);
	#else
		tryToGetValue = ((inipp::Ini<char>*)m_iniMyConfig)->sections[szSection][szKey].c_str();
	#endif
	if(tryToGetValue[0] == '\0')
		pRet->SetFloat(flDefaultValue);
    else
    {
        bool bShouldChange = !pRet->m_pBoundCfg->m_bValueChanged;
        pRet->SetString(tryToGetValue);
        if(bShouldChange) pRet->m_pBoundCfg->m_bValueChanged = false;
    }
	Save();
    pLastEntry = pRet;
	return pRet;
}

ConfigEntry* Config::Bind(const char* szKey, bool bDefaultValue, const char* szSection)
{
	if(!m_bInitialized) return NULL;
	ConfigEntry* pRet = new ConfigEntry;
	pRet->m_pBoundCfg = this;
    strncpy(pRet->m_szMySection, szSection, sizeof(pRet->m_szMySection));
    strncpy(pRet->m_szMyKey, szKey, sizeof(pRet->m_szMyKey));
    pRet->m_szDefaultValue[0] = bDefaultValue ? '1' : '0'; pRet->m_szDefaultValue[1] = 0;
	const char* tryToGetValue;
	#if !defined(__AML) && defined(_ICFG)
		tryToGetValue = m_pICFG->GetValueFrom(m_iniMyConfig, szSection, szKey);
	#else
		tryToGetValue = ((inipp::Ini<char>*)m_iniMyConfig)->sections[szSection][szKey].c_str();
	#endif
	if(tryToGetValue[0] == '\0')
		pRet->SetBool(bDefaultValue);
    else
    {
        bool bShouldChange = !pRet->m_pBoundCfg->m_bValueChanged;
        pRet->SetString(tryToGetValue);
        if(bShouldChange) pRet->m_pBoundCfg->m_bValueChanged = false;
    }
	Save();
    pLastEntry = pRet;
	return pRet;
}

ConfigEntry* Config::BindOnce(const char* szKey, const char* szDefaultValue, const char* szSection)
{
    if(!m_bInitialized) return NULL;
    ConfigEntry entry; ConfigEntry* pRet = &entry;
    pRet->m_pBoundCfg = this;
    strncpy(pRet->m_szMySection, szSection, sizeof(pRet->m_szMySection));
    strncpy(pRet->m_szMyKey, szKey, sizeof(pRet->m_szMyKey));
    strncpy(pRet->m_szDefaultValue, szDefaultValue, sizeof(pRet->m_szDefaultValue));
    const char* tryToGetValue;
    #if !defined(__AML) && defined(_ICFG)
        tryToGetValue = m_pICFG->GetValueFrom(m_iniMyConfig, szSection, szKey);
    #else
        tryToGetValue = ((inipp::Ini<char>*)m_iniMyConfig)->sections[szSection][szKey].c_str();
    #endif
    if(tryToGetValue[0] == '\0')
        pRet->SetString(szDefaultValue);
    else
    {
        bool bShouldChange = !pRet->m_pBoundCfg->m_bValueChanged;
        pRet->SetString(tryToGetValue);
        if(bShouldChange) pRet->m_pBoundCfg->m_bValueChanged = false;
    }
    Save();
    pLastEntry = pRet; // Unsafe!
    return pRet;
}

ConfigEntry* Config::BindOnce(const char* szKey, int nDefaultValue, const char* szSection)
{
    if(!m_bInitialized) return NULL;
    ConfigEntry entry; ConfigEntry* pRet = &entry;
    pRet->m_pBoundCfg = this;
    strncpy(pRet->m_szMySection, szSection, sizeof(pRet->m_szMySection));
    strncpy(pRet->m_szMyKey, szKey, sizeof(pRet->m_szMyKey));
    snprintf(pRet->m_szDefaultValue, sizeof(pRet->m_szDefaultValue), "%d", nDefaultValue);
    const char* tryToGetValue;
    #if !defined(__AML) && defined(_ICFG)
        tryToGetValue = m_pICFG->GetValueFrom(m_iniMyConfig, szSection, szKey);
    #else
        tryToGetValue = ((inipp::Ini<char>*)m_iniMyConfig)->sections[szSection][szKey].c_str();
    #endif
    if(tryToGetValue[0] == '\0')
        pRet->SetInt(nDefaultValue);
    else
    {
        bool bShouldChange = !pRet->m_pBoundCfg->m_bValueChanged;
        pRet->SetString(tryToGetValue);
        if(bShouldChange) pRet->m_pBoundCfg->m_bValueChanged = false;
    }
    Save();
    pLastEntry = pRet; // Unsafe!
    return pRet;
}

ConfigEntry* Config::BindOnce(const char* szKey, float flDefaultValue, const char* szSection)
{
    if(!m_bInitialized) return NULL;
    ConfigEntry entry; ConfigEntry* pRet = &entry;
    pRet->m_pBoundCfg = this;
    strncpy(pRet->m_szMySection, szSection, sizeof(pRet->m_szMySection));
    strncpy(pRet->m_szMyKey, szKey, sizeof(pRet->m_szMyKey));
    snprintf(pRet->m_szDefaultValue, sizeof(pRet->m_szDefaultValue), "%f", flDefaultValue);
    const char* tryToGetValue;
    #if !defined(__AML) && defined(_ICFG)
        tryToGetValue = m_pICFG->GetValueFrom(m_iniMyConfig, szSection, szKey);
    #else
        tryToGetValue = ((inipp::Ini<char>*)m_iniMyConfig)->sections[szSection][szKey].c_str();
    #endif
    if(tryToGetValue[0] == '\0')
        pRet->SetFloat(flDefaultValue);
    else
    {
        bool bShouldChange = !pRet->m_pBoundCfg->m_bValueChanged;
        pRet->SetString(tryToGetValue);
        if(bShouldChange) pRet->m_pBoundCfg->m_bValueChanged = false;
    }
    Save();
    pLastEntry = pRet; // Unsafe!
    return pRet;
}

ConfigEntry* Config::BindOnce(const char* szKey, bool bDefaultValue, const char* szSection)
{
    if(!m_bInitialized) return NULL;
    ConfigEntry entry; ConfigEntry* pRet = &entry;
    pRet->m_pBoundCfg = this;
    strncpy(pRet->m_szMySection, szSection, sizeof(pRet->m_szMySection));
    strncpy(pRet->m_szMyKey, szKey, sizeof(pRet->m_szMyKey));
    pRet->m_szDefaultValue[0] = bDefaultValue ? '1' : '0'; pRet->m_szDefaultValue[1] = 0;
    const char* tryToGetValue;
    #if !defined(__AML) && defined(_ICFG)
        tryToGetValue = m_pICFG->GetValueFrom(m_iniMyConfig, szSection, szKey);
    #else
        tryToGetValue = ((inipp::Ini<char>*)m_iniMyConfig)->sections[szSection][szKey].c_str();
    #endif
    if(tryToGetValue[0] == '\0')
    {
        pRet->SetBool(bDefaultValue);
    }
    else
    {
        bool bShouldChange = !pRet->m_pBoundCfg->m_bValueChanged;
        pRet->SetString(tryToGetValue);
        if(bShouldChange) pRet->m_pBoundCfg->m_bValueChanged = false;
    }
    Save();
    pLastEntry = pRet; // Unsafe!
    return pRet;
}

void ConfigEntry::SetString(const char* newValue)
{
    if(m_bLoadedData && str_equal(newValue, m_szValue)) return;
    
    strncpy(m_szValue, newValue, sizeof(m_szValue)-1); m_szValue[sizeof(m_szValue)-1] = 0;
	m_nIntegerValue = atoi(m_szValue);
	m_fFloatValue = (float)atof(m_szValue);
    
    m_pBoundCfg->m_bValueChanged = true;
    m_bLoadedData = true;

	#if !defined(__AML) && defined(_ICFG)
		m_pBoundCfg->m_pICFG->SetValueTo(m_pBoundCfg->m_iniMyConfig, m_szMySection, m_szMyKey, m_szValue);
	#else
		((inipp::Ini<char>*)(m_pBoundCfg->m_iniMyConfig))->sections[m_szMySection][m_szMyKey] = m_szValue;
	#endif
}

void ConfigEntry::GetString(char* str, size_t len)
{
    strncpy(str, GetString(), len);
}

void ConfigEntry::SetFloat(float newValue)
{
    if(m_bLoadedData && m_fFloatValue == newValue) return;
    
	m_fFloatValue = newValue;
    m_nIntegerValue = (int)newValue;
    snprintf(m_szValue, sizeof(m_szValue), "%f", newValue);
    
    m_pBoundCfg->m_bValueChanged = true;
    m_bLoadedData = true;

	#if !defined(__AML) && defined(_ICFG)
		m_pBoundCfg->m_pICFG->SetValueTo(m_pBoundCfg->m_iniMyConfig, m_szMySection, m_szMyKey, m_szValue);
	#else
		((inipp::Ini<char>*)(m_pBoundCfg->m_iniMyConfig))->sections[m_szMySection][m_szMyKey] = m_szValue;
	#endif
}

void ConfigEntry::SetInt(int newValue)
{
    if(m_bLoadedData && m_nIntegerValue == newValue) return;
    
	m_fFloatValue = (float)newValue;
    m_nIntegerValue = newValue;
	snprintf(m_szValue, sizeof(m_szValue), "%d", newValue);
    
    m_pBoundCfg->m_bValueChanged = true;
    m_bLoadedData = true;

	#if !defined(__AML) && defined(_ICFG)
		m_pBoundCfg->m_pICFG->SetValueTo(m_pBoundCfg->m_iniMyConfig, m_szMySection, m_szMyKey, m_szValue);
	#else
		((inipp::Ini<char>*)(m_pBoundCfg->m_iniMyConfig))->sections[m_szMySection][m_szMyKey] = m_szValue;
	#endif
}

void ConfigEntry::SetBool(bool newValue)
{
    if(m_bLoadedData && m_nIntegerValue == newValue?1:0) return;
    
	m_fFloatValue = newValue?1.0f:0.0f;
    m_nIntegerValue = newValue?1:0;
    m_szValue[0] = newValue ? '1' : '0'; m_szValue[1] = 0;
    
    m_pBoundCfg->m_bValueChanged = true;
    m_bLoadedData = true;

	#if !defined(__AML) && defined(_ICFG)
		m_pBoundCfg->m_pICFG->SetValueTo(m_pBoundCfg->m_iniMyConfig, m_szMySection, m_szMyKey, m_szValue);
	#else
		((inipp::Ini<char>*)(m_pBoundCfg->m_iniMyConfig))->sections[m_szMySection][m_szMyKey] = m_szValue;
	#endif
}

inline bool IsRGBValue(int value) { return value >= 0 && value <= 255; }
inline bool IsRGBFloatValue(float value) { return value >= 0 && value <= 1; }
rgba_t ConfigEntry::ParseColor()
{
    int r, g, b, a, sscanfed = sscanf(m_szValue, "%d %d %d %d", &r, &g, &b, &a);
    if(sscanfed == 4 && IsRGBValue(r) && IsRGBValue(g) && IsRGBValue(b) && IsRGBValue(a))
    {
        return rgba_t{(unsigned char)r,(unsigned char)g,(unsigned char)b,(unsigned char)a};
    }
    else if(sscanfed == 3 && IsRGBValue(r) && IsRGBValue(g) && IsRGBValue(b))
    {
        return rgba_t{(unsigned char)r,(unsigned char)g,(unsigned char)b,255};
    }
    else
    {
        float fr, fg, fb, fa;
        sscanfed = sscanf(m_szValue, "%f %f %f %f", &fr, &fg, &fb, &fa);
        if(sscanfed == 4 && IsRGBFloatValue(r) && IsRGBFloatValue(g) && IsRGBFloatValue(b) && IsRGBFloatValue(a))
        {
            return rgba_t{(unsigned char)(255*fr),(unsigned char)(255*fg),(unsigned char)(255*fb),(unsigned char)(255*fa)};
        }
        else if(sscanfed == 3 && IsRGBFloatValue(r) && IsRGBFloatValue(g) && IsRGBFloatValue(b))
        {
            return rgba_t{(unsigned char)(255*fr),(unsigned char)(255*fg),(unsigned char)(255*fb),255};
        }
    }
    return rgba_t{255,255,255,255};
}

void ConfigEntry::SetColor(rgba_t clr, bool asFloat)
{
    m_nIntegerValue = (int)clr.r;
    m_fFloatValue = (float)clr.r;
    if(asFloat) snprintf(m_szValue, sizeof(m_szValue), "%.3f %.3f %.3f %.3f", (float)(clr.r/255.0f), (float)(clr.g/255.0f), (float)(clr.b/255.0f), (float)(clr.a/255.0f));
    else snprintf(m_szValue, sizeof(m_szValue), "%d %d %d %d", (int)clr.r, (int)clr.g, (int)clr.b, (int)clr.a);
    
    // Kinda expensive to parse the color every time
    // Why do you may want it to be changed automatically anyway?
    m_pBoundCfg->m_bValueChanged = true;
    m_bLoadedData = true;

    #if !defined(__AML) && defined(_ICFG)
        m_pBoundCfg->m_pICFG->SetValueTo(m_pBoundCfg->m_iniMyConfig, m_szMySection, m_szMyKey, m_szValue);
    #else
        ((inipp::Ini<char>*)(m_pBoundCfg->m_iniMyConfig))->sections[m_szMySection][m_szMyKey] = m_szValue;
	#endif
}
