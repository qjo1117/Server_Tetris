#ifndef IPLUGIN_H_
#define IPLUGIN_H_

#define PLUGIN_EXPORTS

#ifdef PLUGIN_EXPORTS
#define PLUGINDECL __declspec(dllexport)
#else
#define PLUGINDECL __declspec(dllimport)
#endif


#include <string>
using namespace std;



class PluginManager;

class IPlugin
{
public:
	virtual ~IPlugin() { }

	virtual PLUGINDECL bool Init() { return false; }
	virtual PLUGINDECL bool Update() { return false; }
	virtual PLUGINDECL bool Render() { return false; }
	virtual PLUGINDECL bool End() { return false; }
	virtual PLUGINDECL const std::string& GetName() { return "IPlugin"; }
};

typedef IPlugin* (*CREATEPLUGIN)(PluginManager& manager);

#define CREATE_PLUGIN(Plugin)											\
extern "C" PLUGINDECL IPlugin* CreatePlugin(PluginManager& manager)				\
{																		\
	return new Plugin(manager);											\
}												  


#endif