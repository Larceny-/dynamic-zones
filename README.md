## dynamicgz

A SA:MP library made to extend and simplify the creation of gang zones.

### Example

```c
new gZone;

main()
{
    gZone = CreateDynamicZone(1755.348144, -2113.468750, 1845.54753, -2054.13245, COLOR_BLUE);
}

// Called when a player enters a zone.
public OnPlayerEnterDynamicZone(playerid, zoneid)
{
    // Check for the zone.
    switch(zoneid)
    {
        case gZone:
        {
            // Check if the player cannot enter the zone
            if(!IsPlayerAdmin(playerid))
            {
                SetPlayerHealth(playerid, 0.0);
                SendClientMessage(playerid, COLOR_WARNING, "* You are not allowed to enter this zone.");
            }
        }
    }
    return 1;
}

// Called when a player leaves a zone.
public OnPlayerLeaveDynamicZone(playerid, zoneid)
{
    // Check for the zone.
    switch(zoneid)
    {
        case gZone:
        {
            if(IsPlayerAdmin(playerid))
            {
                SendClientMessage(playerid, COLOR_ADMIN, "* You left the zone, come back soon!");
            }
        }
    }
    return 1;
}
```

### Functions
Check the list with functions [here](https://github.com/Wuzi/dynamicgz/wiki/Documentation)
