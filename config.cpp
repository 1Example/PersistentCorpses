class CfgPatches
{
    class PersistentCorpses
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data"};
    };
};

class CfgMods
{
    class PersistentCorpses
    {
        dir = "PersistentCorpses";
        picture = "";
        hideName = 0;
        hidePicture = 1;
        name = "Persistent Corpses";
        credits = "OVG Team";
        author = "1Example";
        description = "Keeps corpses from despawning after a restart.";
        version = "1.0";
        type = "mod";

        class defs
        {
            class gameScriptModule
            {
                value = "";
                files[] = {"PersistentCorpses/scripts/3_Game"};
            };
            class worldScriptModule
            {
                value = "";
                files[] = {"PersistentCorpses/scripts/4_World"};
            };
        };
    };
};
