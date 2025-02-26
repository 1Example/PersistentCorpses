class PersistentCorpsesConfig
{
    static int corpseLifetime = 3600; // Default: 1 hour (in seconds)
    
    static void Load()
    {
        string path = "$profile:PersistentCorpses/config.json";

        if (FileExist(path))
        {
            ref PersistentCorpsesConfig tempConfig = new PersistentCorpsesConfig();
            JsonFileLoader<PersistentCorpsesConfig>.JsonLoadFile(path, tempConfig);
            
            corpseLifetime = tempConfig.corpseLifetime;
            Print("[PersistentCorpses] Config loaded successfully. Corpse lifetime: " + corpseLifetime + " seconds");
        }
        else
        {
            Print("[PersistentCorpses] Config not found! Creating default config...");
            if (!FileExist("$profile:PersistentCorpses/"))
                MakeDirectory("$profile:PersistentCorpses/");
            Save();
        }
    }

    static void Save()
    {
        string path = "$profile:PersistentCorpses/config.json";
        
        ref PersistentCorpsesConfig tempConfig = new PersistentCorpsesConfig();
        tempConfig.corpseLifetime = corpseLifetime;
        
        JsonFileLoader<PersistentCorpsesConfig>.JsonSaveFile(path, tempConfig);
        Print("[PersistentCorpsesConfig] Config saved: " + path);
    }
};