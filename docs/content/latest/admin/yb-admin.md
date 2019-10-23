---
title: yb-admin
linkTitle: yb-admin
description: yb-admin
menu:
  latest:
    identifier: yb-admin
    parent: admin
    weight: 2410
isTocNested: true
showAsideToc: true
---

The `yb-admin` utility, located in the `bin` directory of YugabyteDB home, provides a command line interface for administering clusters.

It invokes the [`yb-master`](../admin/yb-master/) and [`yb-tserver`](../admin/yb-tserver/) binaries to perform the necessary administration.

## Online help

Run `yb-admin --help` to display the online help.

```sh
$ ./bin/yb-admin --help
```

## Syntax

To use the `yb-admin` utility from the YugabyteDB home directory, run `./bin/yb-admin` using the following syntax.

```sh
yb-admin [ -master_addresses server1:port,server2:port,server3:port,... ]  [ -timeout_ms <millisec> ] [ -certs_dir_name <dir_name> ] <command> [ command_options ]
```

- *master_addresses*: Comma-separated list of YB-Master hosts and ports. Default value is `localhost:7100`.
- timeout_ms: The RPC timeout, in milliseconds. Default value is `60000`. A value of `0` means don't wait; `-1` means wait indefinitely.
- certs_dir_name: The directory with certificates to use for secure server connections. Default value is `""`.
- *command*: The operation to be performed. See command for syntax details and examples.
- *command_options*: Configuration options, or flags, that can be applied to the command. 

## Commands

- [Universe and cluster](#universe-and-cluster)
- [Table](#table)
- [Backup and snapshot](#backup-and-snapshot)
- [Deployment topology](#deployment-topology)
  - [Multi-zone and multi-region](#multi-zone-and-multi-region)
  - [Master-slave](#master-slave)
  - [Read replica](#read-replica)
- [Change data capture (CDC)](#change-data-capture-cdc)
- [Decommissioning](#decommissioning)
- [Rebalancing](#rebalancing)
- [Security](#security)
  - [Encryption at rest](#encryption-at-rest)

---

### Universe and cluster

#### get_universe_config

Gets the configuration for the universe.

##### Syntax

```
yb-admin -master_addresses get_universe_config
```

#### change_config

Changes the configuration of a tablet.

##### Syntax

```
yb-admin change_config <tablet_id> [ ADD_SERVER | REMOVE_SERVER ] <peer_uuid> [ PRE_VOTER | PRE_OBSERVER ]
```

- *tablet_id*: The identifier (ID) of the tablet.
- ADD SERVER | REMOVE SERVER: Subcommand to add or remove the server.
- *peer_uuid*: The UUID of the peer.
- PRE_VOTER | PRE_OBSERVER: Role of the new peer joining the quorum. Required when using the `ADD_SERVER` subcommand.

#### list_tablet_servers

##### Syntax

```
yb-admin list_tablet_servers <tablet_id>
```

- *tablet_id*: The identifier (ID) of the tablet.

#### list_tablets

Lists all tablets and their replica locations for a particular table.

Useful to find out who the LEADER of a tablet is.

##### Syntax

```sh
yb-admin list_tablets <keyspace> <table_name> [max_tablets]
```

- *keyspace*: The namespace, or name of the database or keyspace.
- *table_name*: The name of the table.
- *max_tablets*: The maximum number of tables to be returned. Default is `10`. Set to `0` to return all tablets.

##### Example

```sh
$ ./bin/yb-admin list_tablets ydb test_tb 0
```

```
Tablet UUID                       Range                                                     Leader
cea3aaac2f10460a880b0b4a2a4b652a  partition_key_start: "" partition_key_end: "\177\377"     127.0.0.1:9100
e509cf8eedba410ba3b60c7e9138d479  partition_key_start: "\177\377" partition_key_end: ""
```

#### list_all_tablet_servers

Lists all tablet servers.

##### Syntax

```
yb-admin list_all_tablet_servers
```

#### list_all_masters

Displays a list of all YB-Master nodes in a table listing the master UUID, RPC host and port, state, and role.

##### Syntax

```
yb-admin -master_addresses list_all_masters
```

- *master_addresses*: List of the YB-Master nodes. Default value is `localhost:7100`.

##### Example

```sh
$ ./bin/yb-admin -master_addresses node7:7100,node8:7100,node9:7100 list_all_masters
```

```
Master UUID         RPC Host/Port          State      Role
...                   node8:7100           ALIVE     FOLLOWER
...                   node9:7100           ALIVE     FOLLOWER
...                   node7:7100           ALIVE     LEADER
```

#### change_master_config

Changes the master configuration.

##### Syntax

```
yb-admin change_master_config <ADD_SERVER|REMOVE_SERVER> <ip_addr> <port> <0|1>
```

- ADD_SERVER | REMOVE_SERVER: Adds or removes a new YB-Master node.
  - After adding or removing a node, verify the status of the YB-Master on the YB-Master UI page (http://node-ip:7000) or run the [`yb-admin dump_masters_state` command](#dump-masters-state).
- *ip_addr*: The IP address of the server node.
- *port*: The port of the server node.
- `0` | `1`: Disabled (`0`) or enabled (`0`). Default is `1`.

#### dump_masters_state

Prints the status of the YB-Master nodes.

##### Syntax

```sh
yb-admin dump_masters_state
```

#### list_tablet_server_log_locations

List the locations of the tablet server logs.

##### Syntax

```sh
yb-admin list_tablet_server_log_locations
```

#### list_tablets_for_tablet_server

Lists all tablets for the specified tablet server.

##### Syntax

```
yb-admin list_tablets_for_tablet_server <ts_uuid>
```

- *ts_uuid*: The UUID of the tablet server.

---

### Table

#### list_tables

Prints a list of all tables.

##### Syntax

```
yb-admin list_tables
```

Returns tables in the following format:

```
<namespace>.<table_name>
```

- *namespace*: Name of the database or keyspace.
- *table_name*: Name of the table.

##### Example

```sh
$ ./bin/yb-admin list_tables
```
```
...
yugabyte.pg_range
template1.pg_attrdef
template0.pg_attrdef_adrelid_adnum_index
template1.pg_conversion
system_platform.pg_opfamily
postgres.pg_opfamily_am_name_nsp_index
system_schema.functions
template0.pg_statistic
system.local
template1.pg_inherits_parent_index
template1.pg_amproc
system_platform.pg_rewrite
yugabyte.pg_ts_config_cfgname_index
template1.pg_trigger_tgconstraint_index
template1.pg_class
template1.pg_largeobject
system_platform.sql_parts
template1.pg_inherits
...
```

#### list_tables_with_db_types

Prints a list of all tables, prefixed by the database type (`ysql`, `ycql`, or `yedis`)

##### Syntax

```
yb-admin list_tables_with_db_types
```

Returns tables in the following format:

```
<db_type>.<namespace>.<table_name>
```

##### Example

```sh
$ ./bin/yb-admin list_tables_with_db_types
```

```
ycql.system_schema.indexes
ycql.system_schema.keyspaces
ycql.system_schema.sys.catalog
ycql.system_schema.tables
ycql.system_schema.triggers
ycql.system_schema.types
ycql.system_schema.views
ysql.postgres.sql_features
ysql.postgres.sql_implementation_info
ysql.postgres.sql_languages
ysql.postgres.sql_packages
...
```

---

### Backup and snapshot 

### list_snapshots

Lists the snapshots and current status.

You can use this command to see when snapshot creation is finished.

#### Syntax

```
yb-admin list_snapshots
```

#### Example

```sh
$ ./bin/yb-admin list_snapshots
```

```
Snapshot UUID                    	State
4963ed18fc1e4f1ba38c8fcf4058b295 	COMPLETE
```

### create_snapshot

Creates a snapshot.

#### Syntax

```
yb-admin create_snapshot <keyspace> <table_name> [<keyspace> <table_name>]... [flush_timeout_in_seconds]
```

- *keyspace*: Specifies the database or keyspace.
- *table_name*: Specifies the table name.
- *flush_timeout_in_seconds*: Specifies duration, in seconds, before flushing snapshot. Default value is `60`. Set to `0` to skip flushing.

#### Example

```
$ ./bin/yb-admin create_snapshot ydb test_tb
```

```
Started flushing table ydb.test_tb
Flush request id: fe0db953a7a5416c90f01b1e11a36d24
Waiting for flushing...
Flushing complete: SUCCESS
Started snapshot creation: 4963ed18fc1e4f1ba38c8fcf4058b295
```

To see if the snapshot creation has finished, run the [`yb-admin list_snapshots`](#list_snapshots) command.

### restore_snapshot

Restores the specified snapshot.

#### Syntax

```
yb-admin restore_snapshot <snapshot_id>
```

### export_snapshot

Generates a metadata file for the given snapshot, listing all the relevant internal UUIDs for various objects (table, tablet, etc.).

#### Syntax

```sh
yb-admin export_snapshot <snapshot_id> <file_name>
```

- *snapshot_id*: Identifier (ID) for the snapshot.
- *file_name*: Name of the the file to contain the metadata. Recommended file extension is `.snapshot`.

#### Example

```sh
$ ./bin/yb-admin export_snapshot 4963ed18fc1e4f1ba38c8fcf4058b295 test_tb.snapshot
```

```
Exporting snapshot 4963ed18fc1e4f1ba38c8fcf4058b295 (COMPLETE) to file test_tb.snapshot
Snapshot meta data was saved into file: test_tb.snapshot
```

### import_snapshot

Imports the specified snapshot metadata file.

#### Syntax

```sh
yb-admin import_snapshot <file_name> [<keyspace> <table_name> [<keyspace> <table_name>]...]
```

- *file_name*: The name of the snapshot file to import
- *keyspace*: The name of the database or keyspace
- *table_name*: The name of the table

{{< note title="Note" >}}

The *keyspace* and the *table* can be different from the exported one.

{{< /note >}}

#### Example

```sh
$ ./bin/yb-admin import_snapshot test_tb.snapshot ydb test_tb
```

```
Read snapshot meta file test_tb.snapshot
Importing snapshot 4963ed18fc1e4f1ba38c8fcf4058b295 (COMPLETE)
Target imported table name: ydb.test_tb
Table being imported: ydb.test_tb
Successfully applied snapshot.
Object           	Old ID                           	New ID                          
Keyspace         	c478ed4f570841489dd973aacf0b3799 	c478ed4f570841489dd973aacf0b3799
Table            	ff4389ee7a9d47ff897d3cec2f18f720 	ff4389ee7a9d47ff897d3cec2f18f720
Tablet 0         	cea3aaac2f10460a880b0b4a2a4b652a 	cea3aaac2f10460a880b0b4a2a4b652a
Tablet 1         	e509cf8eedba410ba3b60c7e9138d479 	e509cf8eedba410ba3b60c7e9138d479
Snapshot         	4963ed18fc1e4f1ba38c8fcf4058b295 	4963ed18fc1e4f1ba38c8fcf4058b295
```

### delete_snapshot

Deletes the snapshot information, usually cleaned up at the end, since this is supposed to be a transient state.

#### Syntax

```
yb-admin delete_snapshot <snapshot_id>
```

---

### Deployment topology

#### Multi-zone and multi-region

##### modify_placement_info

Modifies the placement information (cloud, region, and zone) for a deployment.

###### Syntax

```sh
yb-admin modify_placement_info <placement_info> <replication_factor>
```

- *placement_info*: Comma-delimited list of placements for *cloud*.*region*.*zone*. Default value is `cloud1.datacenter1.rack1`.
- *replication_factor*: The number of replicas for each tablet.

###### Example

```sh
$ ./bin/yb-admin yb-admin --master_addresses $MASTER_RPC_ADDRS \
    modify_placement_info  \
    aws.us-west.us-west-2a,aws.us-west.us-west-2b,aws.us-west.us-west-2c 3
```

You can verify the new placement information by running the following `curl` command:

```sh
$ curl -s http://<any-master-ip>:7000/cluster-config
```

##### set_preferred_zones

Sets the preferred zones.

###### Syntax

```
yb-admin set_preferred_zones <cloud.region.zone> [<cloud.region.zone>]...
```

### list_replica_type_counts

Prints a list of replica types and counts for the specified table.

#### Syntax

```
yb-admin list_replica_type_counts <keyspace> <table_name>
```

- *keyspace*: The name of the database or keyspace.
- *table_name*: The name of the table.

#### Master-slave

##### setup_universe_replication

###### Syntax

```
yb-admin setup_universe_replication <producer_universe_uuid> <producer_master_addresses> <comma_separated_list_of_table_ids>
```

- *producer_universe_uuid*: The UUID of the producer universe.
- *producer_master_addresses*: Comma-separated list of master producer addresses.
- *comma_separated_list_of_table_ids*: Comma-separated list of table identifiers (IDs).

##### delete_universe_replication <producer_universe_uuid>

Deletes universe replication for the specified producer universe.

###### Syntax

```
yb-admin delete_universe_replication <producer_universe_uuid>
```

- *producer_universe_uuid*: The UUID of the producer universe.

##### set_universe_replication_enabled

Sets the universe replication to be enabled or disabled.

###### Syntax

```
yb-admin set_universe_replication_enabled <producer_universe_uuid>
```

- *producer_universe_uuid*: The UUID of the producer universe.
- `0` | `1`: Disabled (`0`) or enabled (`1`). Default is `1`.

#### Read replica

##### add_read_replica_placement_info

Add a read replica cluster to the master configuration.

###### Syntax

```
yb-admin add_read_replica_placement_info <placement_info> <replication_factor>
```

- *placement_info*: A comma-delimited list of placements for *cloud*.*region*.*zone*. Default value is `cloud1.datacenter1.rack1`.
- *replication_factor*: The number of replicas.

##### modify_read_replica_placement_info

###### Syntax

```
yb-admin modify_read_replica_placement_info <placement_info> <replication_factor> [ <placement_uuid> ]
```

- *placement_info*: A comma-delimited list of placements for *cloud*.*region*.*zone*. Default value is `cloud1.datacenter1.rack1`.
- *replication_factor*: The number of replicas.
- *placement_uuid*: The UUID of the read replica cluster.

##### delete_read_replica_placement_info

Delete the read replica.

###### Syntax

```
yb-admin delete_read_replica_placement_info <placement_uuid>
```

- *placement_uuid*: The UUID of the read replica cluster.

---

### Security

#### Encryption at rest

##### rotate_universe_key

Rotates the universe key.

###### Syntax

```
yb-admin rotate_universe_key <key_path>
```

- *key_path*: The path of the universe key.

###### Example

```sh
$ ./bin/yb-admin -master_addresses ip1:7100,ip2:7100,ip3:7100 rotate_universe_key
/mnt/d0/yb-data/master/universe_key_2
```

##### disable_encryption

Disables cluster-wide encryption.

###### Syntax

```
yb-admin disable_encryption
```

Returns the message:

```
Encryption status: DISABLED
```

###### Example

```sh
$ ./bin/yb-admin -master_addresses ip1:7100,ip2:7100,ip3:7100 disable_encryption
```

```
Encryption status: DISABLED
```

##### is_encryption_enabled

Checks if cluster-wide encryption is enabled.

###### Syntax

```
yb-admin is_encryption_enabled
```

Returns message:

```
Encryption status: ENABLED with key id <key_id_2>
```

The new key ID (`<key_id_2>`) should be different from the previous one (`<key_id>`).

###### Example

```sh
$ ./bin/yb-admin -master_addresses ip1:7100,ip2:7100,ip3:7100 is_encryption_enabled
```

```
Encryption status: ENABLED with key id <key_id_2>
```

### Change data capture (CDC)

#### create_cdc_stream

Creates a change data capture (CDC) stream for the specified table.

##### Syntax

```
yb-admin create_cdc_stream <table_id>
```

- *table_id*: The identifier (ID) of the table.

---

### Decommissioning

### get_leader_blacklist_completion

Gets the tablet load move completion percentage for blacklisted nodes.

#### Syntax

```
yb-admin get_leader_blacklist_completion
```

#### Example

```sh
$ ./bin/yb-admin get_leader_blacklist_completion
```

### change_blacklist

Changes the blacklist for YB-TServers.

After old YB-TServer nodes are terminated, you can use this command to clean up the blacklist.

#### Syntax

```
yb-admin change_blacklist <ADD|REMOVE> <ip_addr>:<port> [<ip_addr>:<port>]...
```

- ADD | REMOVE: Adds or removes the specified YB-TServers.
- *ip_addr:port*: The IP address and port of the YB-TServer.

#### Example

```sh
$ ./bin/yb-admin -master_addresses $MASTERS change_blacklist ADD node1:9100 node2:9100 node3:9100 node4:9100 node5:9100 node6:9100
```

### change_leader_blacklist

#### Syntax

```
yb-admin change_leader_blacklist <ADD | REMOVE> <ip_addr>:<port> [<ip_addr>:<port>]...
```

- ADD | REMOVE: Adds or removes nodes from blacklist.
- *ip_addr*: The IP address of the node.
- *port*: The port of the node.

---

### Rebalancing

#### set_load_balancer_enabled

Enables or disables the load balancer.

##### Syntax

```
yb-admin set_load_balancer_enabled 0 | 1
```

- `0` | `1`: Enabled (`1`) is the default. To disable, set to `0`.

##### Example

```sh
./bin/yb-admin set_load_balancer_enabled 0
```

#### get_load_move_completion

Checks the percentage completion of the data move.

You can rerun this command periodically until the value reaches `100.0`, indicating that the data move has completed.

##### Syntax

```
yb-admin get_load_move_completion
```

{{< note title="Note" >}}

The time needed to complete a data move depends on the following:

- number of tablets and tables
- size of each of those tablets
- SSD transfer speeds
- network bandwidth between new nodes and existing ones

{{< /note >}}

For an example of performing a data move and the use of this command, see [Change cluster configuration](../manage/change-cluster-config).

##### Example

In the following example, the data move is `66.6` percent done.

```sh
$ ./bin/yb-admin get_load_move_completion
```

Returns the following percentage:

```
66.6
```

#### get_is_load_balancer_idle

Finds out if the load balancer is idle.

##### Syntax

```
yb-admin get_is_load_balancer_idle
```

##### Example

```sh
./bin/yb-admin get_is_load_balancer_idle
```