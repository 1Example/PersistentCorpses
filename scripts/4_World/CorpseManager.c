class CorpseManager
{
    static ref array<ref PCCorpseData> loadedCorpses;
    static string CORPSE_SAVE_PATH = "$profile:PersistentCorpses/corpse_data.json";

    static void OnInit()
    {
        PersistentCorpsesConfig.Load();
        
        Print("[PersistentCorpses] Loading corpse persistence system...");
        
        loadedCorpses = new array<ref PCCorpseData>();

        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CorpseManager.LoadFromDisk, 1000, false);
        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CorpseManager.RestoreCorpses, 2000, false);
    }

    static void LoadFromDisk()
    {
        Print("[PersistentCorpses] Loading corpses from disk...");

        if (FileExist(CORPSE_SAVE_PATH))
        {
            if (!loadedCorpses)
                loadedCorpses = new array<ref PCCorpseData>();
            else
                loadedCorpses.Clear();
                
            JsonFileLoader<array<ref PCCorpseData>>.JsonLoadFile(CORPSE_SAVE_PATH, loadedCorpses);
            Print("[PersistentCorpses] Loaded " + loadedCorpses.Count() + " corpses.");
        }
    }

    static void SaveToDisk()
    {
        Print("[PersistentCorpses] Saving all corpse data...");
        
        // Create directory if it doesn't exist
        if (!FileExist("$profile:PersistentCorpses/"))
            MakeDirectory("$profile:PersistentCorpses/");
            
        // Save all corpses to disk
        JsonFileLoader<array<ref PCCorpseData>>.JsonSaveFile(CORPSE_SAVE_PATH, loadedCorpses);
    }

    static void SaveCorpse(ref PCCorpseData corpse)
    {
        if (!loadedCorpses)
            loadedCorpses = new array<ref PCCorpseData>();
            
        loadedCorpses.Insert(corpse);
        SaveToDisk();
    }

    static void RestoreCorpses()
    {
        Print("[PersistentCorpses] Restoring corpses to the world...");

        if (!loadedCorpses)
            return;

        array<ref PCCorpseData> validCorpses = new array<ref PCCorpseData>();
            
        for (int i = 0; i < loadedCorpses.Count(); i++)
        {
            ref PCCorpseData corpse = loadedCorpses.Get(i);
            
            // Check if the corpse is expired
            if (corpse.IsExpired())
            {
                Print("[PersistentCorpses] Skipping expired corpse at position: " + corpse.position.ToString());
                continue;
            }

            SpawnCorpse(corpse);
            validCorpses.Insert(corpse);
        }
        
        // Update the array to only contain valid corpses
        loadedCorpses = validCorpses;
        SaveToDisk();
    }

    static void SpawnCorpse(ref PCCorpseData corpse)
    {
        vector spawnPosition = corpse.position;

        Print("[PersistentCorpses] Spawning corpse at position: " + spawnPosition.ToString());

        EntityAI spawnedCorpse = EntityAI.Cast(GetGame().CreateObject(corpse.characterType, spawnPosition, false, true));
        if (!spawnedCorpse)
        {
            Print("[PersistentCorpses] ERROR: Failed to spawn corpse!");
            return;
        }

        // Restore items
        for (int i = 0; i < corpse.inventory.Count(); i++)
        {
            string itemName = corpse.inventory[i];
            Print("[PersistentCorpses] Restoring item: " + itemName);

            EntityAI itemEntity = EntityAI.Cast(spawnedCorpse.GetInventory().CreateInInventory(itemName));
        }
    }

    static void OnPlayerDeath(PlayerBase player)
    {
        if (!player || !player.GetIdentity())
            return;
            
        string playerName = player.GetIdentity().GetName();
        string characterType = player.GetType();
        
        array<string> itemNames = new array<string>();
        array<EntityAI> items = new array<EntityAI>();
        player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);
        
        for (int i = 0; i < items.Count(); i++)
        {
            EntityAI item = items.Get(i);
            if (item && item.GetType() != "")
            {
                itemNames.Insert(item.GetType());
            }
        }

        vector position = player.GetPosition();
        int timestamp = GetGame().GetTime();
        ref PCCorpseData corpse = new PCCorpseData(position, playerName, characterType, itemNames, timestamp);

        SaveCorpse(corpse);
        Print("[PersistentCorpses] Saved corpse for player: " + playerName);
    }
}