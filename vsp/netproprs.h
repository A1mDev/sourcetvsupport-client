#ifndef _INCLUDE_NETPROPS_VSP_H_
#define _INCLUDE_NETPROPS_VSP_H_

#include <string>
#include <unordered_map>
#include <stdexcept>

#include "dt_recv.h"
#include "dt_common.h"
#include "client_class.h"

class CNetPropsManager
{
public:
	int DumpClientClasses(ClientClass *pClientClasses)
	{
		for (ClientClass* pCur = pClientClasses; pCur; pCur = pCur->m_pNext) {
			if (pCur->m_pRecvTable == NULL) {
				continue;
			}

			DumpNetPropsRecursive(pCur->GetName(), pCur->m_pRecvTable);
		}

		return m_netProps.size();
	}

	void DumpNetPropsRecursive(const char* pClassName, RecvTable *pTable, uint16_t iOffset = 0)
	{
		char sBuffer[128];

		for (int i = 0; i < pTable->m_nProps; i++) {
			RecvProp* pProp = pTable->GetProp(i);

			if (pProp == NULL || isdigit(pProp->GetName()[0])) {
				continue;
			}
			
			if (strcmp(pProp->GetName(), "baseclass") == 0) {
				continue;
			}

			if (pProp->GetType() == DPT_DataTable &&
				pProp->GetDataTable() != NULL &&
				pProp->GetDataTable()->m_pNetTableName[0] == 'D'
			) {
				DumpNetPropsRecursive(pClassName, pProp->GetDataTable(), iOffset + pProp->GetOffset());
			}

			snprintf(sBuffer, sizeof(sBuffer), "%s::%s", pClassName, pProp->GetName());

			/*Msg(VSP_LOG_PREFIX "Netprop %s, type: %s (%d), offset: %d""\n", \
					sBuffer, CNetPropsManager::PropTypeToString(pProp->GetType()), pProp->GetType(), iOffset + pProp->GetOffset());*/

			m_netProps[std::string(sBuffer)] = std::make_pair(pProp->GetType(), iOffset + pProp->GetOffset());
		}
	}

	static const char* PropTypeToString(SendPropType iType)
	{
		static char sText[128];

		switch (iType) {
		case DPT_Int:
			snprintf(sText, sizeof(sText), "int");
			break;
		case DPT_Float:
			snprintf(sText, sizeof(sText), "float");
			break;
		case DPT_Vector:
			snprintf(sText, sizeof(sText), "vector3D");
			break;
		case DPT_VectorXY:
			snprintf(sText, sizeof(sText), "vector2D");
			break;
#if 0 // We can't ship this since it changes the size of DTVariant to be 20 bytes instead of 16 and that breaks MODs!!!
		case DPT_Quaternion:
			Q_snprintf(text, sizeof(text), "vector4D");
			break;
#endif
		case DPT_String:
			snprintf(sText, sizeof(sText), "string");
			break;
		case DPT_Array:
			snprintf(sText, sizeof(sText), "array");
			break;
		case DPT_DataTable:
			snprintf(sText, sizeof(sText), "table");
			break;
		default:
			snprintf(sText, sizeof(sText), "Type %i unknown", iType);
			break;
		}

		return sText;
	}

	uint16_t GetOffset(std::string sPropName)
	{
		uint16_t iOffset = 0;
		
		try {
			iOffset = m_netProps.at(sPropName).second;
		} catch (std::out_of_range& e) {
			const char* sError = (e.what() == NULL) ? "Unknown" : e.what();
			Msg(VSP_LOG_PREFIX "Failed to get offset %s, error: %s ""\n", sPropName, sError);
			iOffset = 0;
		}

		return iOffset;
	}

	inline int GetType(std::string sPropName)
	{
		return m_netProps.at(sPropName).first;
	}

	inline void Clear()
	{
		m_netProps.clear();
	}

private:
	std::unordered_map<std::string, std::pair<SendPropType, uint16_t>> m_netProps;
};

extern CNetPropsManager g_NetProps;

#endif // _INCLUDE_NETPROPS_VSP_H_