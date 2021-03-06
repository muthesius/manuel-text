-- textinstallation - comment receiver script
-- jens alexander ewald, lea.io, 2015

local db = "/root/textinstallation/db/comments.db.csv"
local store = "/root/textinstallation/db/comments.csv"
local debugHost = "192.168.123.207"
local maxComments = 20
local debug = false

-- pick one client to show the current comment next
local clients = {
  "192.168.123.225",
  "192.168.123.162",
  "192.168.123.155",
  "192.168.123.146",
  "192.168.123.210",
  "192.168.123.193",
}

-- Import the JSON en-/decoder
JSON = (loadfile "JSON.lua")()

-- Helper Functions
local hex_to_char = function(x)
  return string.char(tonumber(x, 16))
end
local unescape = function(url)
  return url:gsub("%%(%x%x)", hex_to_char)
end
local lines = function(str)
  local t = {}
  local function helper(line) table.insert(t, line) return "" end
  helper((str:gsub("(.-)\r?\n", helper)))
  return t
end
-- from http://lua-users.org/wiki/CsvUtils
-- Used to escape "'s by toCSV
function escapeCSV (s)
  if string.find(s, '[,"]') then
    s = '"' .. string.gsub(s, '"', '""') .. '"'
  end
  return s
end
-- Convert from CSV string to table (converts a single line of a CSV file)
function fromCSV (s)
  s = s .. ','        -- ending comma
  local t = {}        -- table to collect fields
  local fieldstart = 1
  repeat
    -- next field is quoted? (start with `"'?)
    if string.find(s, '^"', fieldstart) then
      local a, c
      local i  = fieldstart
      repeat
        -- find closing quote
        a, i, c = string.find(s, '"("?)', i+1)
      until c ~= '"'    -- quote not followed by quote?
      if not i then error('unmatched "') end
      local f = string.sub(s, fieldstart+1, i-1)
      table.insert(t, (string.gsub(f, '""', '"')))
      fieldstart = string.find(s, ',', i) + 1
    else                -- unquoted; find next comma
      local nexti = string.find(s, ',', fieldstart)
      table.insert(t, string.sub(s, fieldstart, nexti-1))
      fieldstart = nexti + 1
    end
  until fieldstart > string.len(s)
  return t
end
-- Convert from table to CSV string
function toCSV (tt)
  local s = ""
  -- ChM 23.02.2014: changed pairs to ipairs
  -- assumption is that fromCSV and toCSV maintain data as ordered array
  for _,p in ipairs(tt) do
    s = s .. "," .. escapeCSV(p)
  end
  return string.sub(s, 2)      -- remove first comma
end

-- END Helper Functions

local data = {}
local query = ""

-- Get the current `env`
local handle = io.popen("env")
local result = handle:read("*a")
handle:close()
local env = lines(result)
-- os.execute("logger -t \"kommentar.env\" \"".. result .. "\"\n")

-- Extract the QUERY_STRING (chilli specific)
for i,v in pairs(env) do
  i,j = string.find(v, "QUERY_STRING=")
  if i == 1 then
    query = string.sub(v,j+1,string.len(v))
    query = unescape(query)
  end
end

-- Decode the JSON from the found query
local input = JSON:decode(query)

if input == nil then
  input = {}
  input["name"] = ""
  input["comment"] = ""
end

-- Fix anonymous visitor
if input.name == nil or string.len(input.name) == 0 then
  input.name = "Ein Besucher"
end

data["comment"] = input

-- add the comment to the DB
local f,err,status = io.open(db, "a")

-- Zitat,Name,Position,Jahr,Anlass(which is 'comment' here)
local entry = escapeCSV(input.comment)..","..escapeCSV(input.name)..",,"..os.date("%Y")..",comment\n"
os.execute("logger -t \"kommentar.user.comment.csv\" '" .. entry .. "'\n")

f:write(entry)
f:close()

local commentCount = 0
for _ in io.lines(db) do
  commentCount = commentCount+1
end

os.execute("logger -t \"kommentar.user.comment.csv\" 'Number of comments: " .. commentCount .. "'\n")

local fout,status,err = io.open(store, "w")
local line = 0
local limit = commentCount-maxComments
for _line in io.lines(db) do
  line = line+1
  if commentCount <= maxComments or line > limit then
    -- copy  data set to new file
    fout:write(_line,"\n")
  end
end
fout:write("\n") -- empty line at the end
fout:close()

-- notify clients to get new DB
require 'socket'
port = 3332
udp, err = socket.udp()
if not udp then print(err) os.exit() end

-- udp:setoption('broadcast', true)

-- pick a random client
math.randomseed(os.time())
local clientID = math.floor(math.random() * 5.9)+1

--- overwrite for testing:
if debug then
  clientID = 1
end

-- send signal to show the last user comment
for i,client in ipairs(clients) do
  if i == clientID then
    os.execute("logger -t \"kommentar.network\" 'Sending show comment to "..client.."'\n")
    udp:sendto("showUserComment", client, port)
  else
    os.execute("logger -t \"kommentar.network\" 'Sending update beacon to "..client.."'\n")
    udp:sendto("update", client, port)
  end
end

-- also send to jens' machine for debugging
udp:sendto("update", debugHost, port)
udp:sendto("showUserComment", debugHost, port)


-- respond back with the current comment
local body = JSON:encode_pretty(data)

os.execute("logger -t \"kommentar.user.comment\" '" .. body .. "'\n")


-- Respond with OK and curretn comment
local header = [[HTTP/1.0 200 OK
Content-Type: application/json
Cache: none
Connection: close

]]
print(header .. body .. '\n\n')