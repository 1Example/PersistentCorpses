// This will run when the game starts
void InitPersistentCorpses()
{
    Print("[PersistentCorpses] Registering initialization...");
    
    // Use a delayed call to ensure the server is fully initialized
    GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(CheckServerAndInit, 5000, false);
}

// Check if we're on server and initialize if so
void CheckServerAndInit()
{
    if (GetGame().IsServer())
    {
        Print("[PersistentCorpses] Server detected, initializing...");
        CorpseManager.OnInit();
    }
}

// Hook into player death events without modifying MissionServer
modded class PlayerBase
{
    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);
        
        if (GetGame().IsServer())
        {
            string playerName = "Unknown";
            if (this.GetIdentity())
                playerName = this.GetIdentity().GetName();
                
            Print("[PersistentCorpses] Player death detected: " + playerName);
            CorpseManager.OnPlayerDeath(this);
        }
    }
}