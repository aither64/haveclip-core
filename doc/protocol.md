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
  1. Request: [SerialModeToggle](#serialmodetoggle)
  2. Reply: [Confirm](#confirm)

#### End serial mode
  1. Request: [SerialModeToggle](#serialmodetoggle)
  2. Reply: [Confirm](#confirm)

#### Add item to serial batch
  1. Request: [SerialModeAppendReady](#serialmodeappendready)
  2. Reply: [ClipboardUpdateConfirm](#clipboardupdateconfirm)

Next step depends on status from reply:

  * continue
    3. Request: [ClipboardUpdateSend](#clipboardupdatesend)
    4. Reply: [Confirm](#confirm)
  * abort - close connection
  * not exists / not matches
    * morph into conversation [Copy serial batch](#copy-serial-batch)
  * not understood - close connection

#### Move to next item in serial batch
  1. Request: [SerialModeInfo](#serialmodeinfo)
  2. Reply: [Confirm](#confirm)

Next step depends on status from reply:

  * continue - close connection
  * abort - close connection
  * not exists / not matches
    * morph into conversation [Copy serial batch](#copy-serial-batch)
  * not understood
    - morph into conversation [Clipboard update](#clipboard-update)

#### Copy serial batch
  1. Request: [SerialModeInfo](#serialmodeinfo)  
       For each item command [ClipboardUpdateSend](#clipboardupdatesend)
  2. Reply: [Confirm](#confirm)

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
       4    int8             meaning of the list below (Accept or Exclude)
       -    QStringList      list of mime types

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

#### SerialModeToggle

  Length    Type             Meaning
 -------    -----------      ------------------------------------------------
       8    int64            serial batch ID

#### SerialModeAppendReady

  Length    Type             Meaning
 -------    -----------      ------------------------------------------------
       8    int64            serial batch ID
       4    int32            serial batch item count

#### SerialModeInfo

  Length    Type             Meaning
 -------    -----------      ------------------------------------------------
       8    int64            serial batch ID
       4    int32            serial batch item count
       4    int32            current index
