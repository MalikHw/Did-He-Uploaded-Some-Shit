# Did He Upload Something!

sends a discord webhook when you upload or update a level... that's it

## Setup

1. make a webhook in your discord server (channel settings > integrations > webhooks) (PC or phone browser)
2. copy the webhook URL
3. paste it in the mod settings (full one)
4. done

## Custom Text

enable "Use Custom Text" in settings, then hit "Edit File" to open `customtext.txt` in notepad. (on mobile it'll open mod config folder, you open the file manually ig)

you can use these variables:
- `{creator}` - your username
- `{name}` - level name
- `{id}` - level ID
- `{lengh}` - length (Tiny/Short/Medium/Long/XL/Plat)
- `{objects}` - object count
- `{role}` - role ping (if enabled)
- `{isUploaded"text"}` - only shows on new uploads
- `{isUpdated"text"}` - only shows on updates

just write normally in the file, press enter for new lines.

### discord markdown cheatsheet
```
# Huge header
## big header
### smaller header
**bold**
*italic*
***bold italic***
-# small text
||spoiler||
`code`
```multilines of code```
```

## Role Ping

1. enable developer mode in discord (settings > advanced > developer mode)
2. right click the role > copy role ID
3. paste it in "Role ID" in the mod settings
4. enable "Role Ping"


### Credits:
- [MalikHw47](https://www.youtube.com/@MalikHw47) - Mod creator

Any issues at [Issues](https://github.com/MalikHw/Did-He-Uploaded-Some-Shit/issues)
