# Lumberjack - the simple logger

Lumberjack is a quick, thread safe logger with a fresh, simple API. 


## API Use 
Lumberjack only knows how to do two things, save data in the database, and throw
it into a .csv. Let's look at saving things first.

### Saving
When saving data, Lumberjack listens for a `LogData` data type. The `LogData` 
struct contains a `DataType` (`ACS`, `POWER`, `BEAM_STEERING` supported) and a
`void*` which will contain your spacetype.

### Dumping
When dumping, Lumberjack listens for a `DataType` (`ACS`, `POWER`, 
`BEAM_STEERING`, `ALL`, `NONE`) and will dump to a .csv file based on the table
name. As of now, the log files generated will look like: `acs_log.csv`, 
`power_log.csv`, and, `beam_steering_log.csv`

If someone decides to change the table name and doesn't update the this doc I
will find them and take them down (me included).

For convenience, you can send Lumberjack an `ALL` if you want it to dump all of
its tables. You can also pass it a `NONE` if you want it to pass `NONE` I guess.
Maybe you want to just poke it I guess?


## Supported Teams
Anything in `spacetypes.h` is supposed to be supported. As of right now, that
means `AcsData`, `BeemSteeringData`, and `PowerData`. If the data you're 
attempting to store is not in one of those formats, it will throw your data out
and move on with life.
