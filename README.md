# Snakes

## ✅ TODO List

- [🟢] Minimal compiling version for ASCII view and by player control
- [🟢] Dynamically resizing playing field
- [🟢] Implementation of "greedy" bot algorithm (Aspids)
- [🟢] Implementation of "silly" bot algorithm (Boas)
- [🟢] Tracement of tail's segments direction (cross-view dependance) & nicer ASCII interface
- [🟢] Simplest graphic view (SFML)
- [🔴] Statistics records, tournament like system
- [🔴] *[Optional] Sprites & nicer Graphic interface*

## 🚀 Idea to implement

- [🔴] Dynamic geckos (with shown direction where they go)
- [🔴] *[Optional] Some geckos get free after snake's death*

Progress: [▓▓▓▓▓▓░░░░] 60%


---

# 🔪 Killer feature

**The idea of second chance with an opportunity to regain some length of a previous body back.**

After death, the head will be left on the playing arena, whereas the snake will be respawned. The strategic goal of opponents will be to not let the revived snake to the head for certain amount of time.
Implementation core: exploitation of existing bots' algorithms, by creating around the dropped head N high priority "food" pieces that won't have any physical body on the arena, hence, just guide the bots.


---