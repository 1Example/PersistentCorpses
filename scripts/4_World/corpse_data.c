class CorpseData
{
    vector position;             
    string playerName;           
    string characterType;        
    ref array<string> inventory; 
    int timestamp;               

    void CorpseData(vector pos, string name, string type, array<string> inv, int time)
    {
        position = pos;
        playerName = name;
        characterType = type;
        inventory = new array<string>;
        
        // Copy inventory items
        if (inv)
        {
            for (int i = 0; i < inv.Count(); i++)
            {
                inventory.Insert(inv[i]);
            }
        }
        
        timestamp = time;
    }

    bool IsExpired()
    {
        int currentTime = GetGame().GetTime();
        return (currentTime - timestamp) > PersistentCorpsesConfig.corpseLifetime;
    }
}