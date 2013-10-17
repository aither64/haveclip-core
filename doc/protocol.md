% Binary protocol for communication between instances of HaveClip

Possible conversations
----------------------

### Clipboard update

Requestor is sending ClipboardItem to replier.

  1. Request: [ClipboardUpdateReady](#clipboardupdateready)
  2. Reply: [ClipboardUpdateConfirm](#clipboardupdateconfirm)
  3. Request: [ClipboardUpdateSend](#clipboardupdatesend)
  4. Reply: [Confirm](#confirm)


### Serial clipboard

#### Begin serial mode
  1. Request: [SerialModeBegin](#serialmodebegin)
  2. Reply: [Confirm](#confirm)

#### End serial mode
  1. Request: [SerialModeEnd](#serialmodeend)
  2. Reply: [Confirm](#confirm)

#### Add item to serial batch
  1. Request: [SerialModeAppendReady](#serialmodeappendready)
  2. Reply: [SerialModeAppendAccept](#serialmodeappendaccept)

Next step depends on status from reply:

  * continue
    3. Request: [SerialModeAppend](#serialmodeappend)
    4. Reply: [Confirm](#confirm)
  * abort - close connection
  * not exists / not matches
    3. Request: [SerialModeCopy](#serialmodecopy)
    4. Reply: [Confirm](#confirm)
  * not understood - close connection

#### Move to next item in serial batch
  1. Request: [SerialModeNext](#serialmodenext)
  2. Reply: [Confirm](#confirm)

Next step depends on status from reply:

  * continue - close connection
  * abort - close connection
  * not exists / not matches
    3. Request: [SerialModeCopy](#serialmodecopy)
    4. Reply: [Confirm](#confirm)
  * not understood - close connection

Protocol structure
------------------

### Message header

  Length    Type         Meaning
 -------    -------      ------------------------------------------------
       4    uint32       magic number, identifies correct data format
       4    int32        version
       4    int32        conversation
       4    int32        message type
       8    quint64      message length
       -    QString      password

### Message types

#### ClipboardUpdateReady

  Length    Type         Meaning
 -------    -------      ------------------------------------------------
       4    int32        mode (clipboard, selection, both)

#### ClipboardUpdateConfirm

  Length    Type             Meaning
 -------    -----------      ------------------------------------------------
       4    int32            status (continue/abort)
       -    QStringList      accept only these mime types (empty = all)
       -    QStringList      exclude only these mime types (empty = none)

#### ClipboardUpdateSend

  Length    Type                           Meaning
 -------    -------------------------      ------------------------------------------------
       4    int32                          mode (clipboard, selection, both)
       -    QStringList                    formats
       -    QByteArray * formats.size      mime data

#### Confirm

  Length    Type             Meaning
 -------    -----------      -----------------------------------------------------------
       4    int32            status (ok, abort, not exists, not matches, not understood)

#### SerialModeBegin

  Length    Type             Meaning
 -------    -----------      ------------------------------------------------
       4    int32            serial batch ID

#### SerialModeEnd

  Length    Type             Meaning
 -------    -----------      ------------------------------------------------
       4    int32            serial batch ID

#### SerialModeAppendReady

  Length    Type             Meaning
 -------    -----------      ------------------------------------------------
       4    int32            serial batch ID
       4    int32            serial batch item count

#### SerialModeAppendAccept

  Length    Type             Meaning
 -------    -----------      ------------------------------------------------
       4    int32            status (continue, abort, not exists, not matches, not understood)
       -    QStringList      accept only these mime types (empty = all)
       -    QStringList      exclude only these mime types (empty = none)

#### SerialModeAppend

  Length    Type                           Meaning
 -------    -------------------------      ------------------------------------------------
       4    int32                          mode (clipboard, selection, both)
       -    QStringList                    formats
       -    QByteArray * formats.size      mime data


#### SerialModeCopy

  Length    Type             Meaning
 -------    -----------      ------------------------------------------------
       4    int32            serial batch ID
       4    int32            serial batch item count
       4    int32            current index

For each item:

  Length    Type                           Meaning
 -------    -------------------------      ------------------------------------------------
       4    int32                          mode (clipboard, selection, both)
       -    QStringList                    formats
       -    QByteArray * formats.size      mime data


#### SerialModeNext

  Length    Type             Meaning
 -------    -----------      ------------------------------------------------
       4    int32            serial batch ID
       4    int32            serial batch item count
       4    int32            current index
