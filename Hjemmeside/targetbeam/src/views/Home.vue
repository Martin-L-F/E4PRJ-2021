<template>
  <div class="page">
    <div class="title">
      <h1>TARGET<span class="focus-color">BEAM</span></h1>
    </div>
    <div style="align-self: stretch; flex-grow: 2" class="box home-content">
      <h1>Users</h1>
      <div class="flex-row">
        <user
          :email="user.email"
          :name="user.name"
          v-for="user in users"
          :key="user.email"
        />
        <addUser />
      </div>
      <h1>Pistols</h1>
      <div class="flex-row">
        <div
          class="box pistol"
          v-for="pistol in pistols"
          :key="pistol.ID"
          :style="'--i:' + pistol.battery"
          :class="{ error: pistol.battery < 20, good: pistol.charging }"
        >
          <h3>Pistol {{ pistol.ID }}</h3>
          <p>Charge: {{ pistol.battery }}%</p>
          <p>Charging: {{ pistol.charging }}</p>
        </div>
      </div>
      <div class="grow"></div>

      <transition name="fade" mode="out-in">
        <div
          class="btn"
          :class="{
            disabled: users.length != pistols.length || users.length <= 0,
            loading: loadingStartGame
          }"
          v-on:click="prepareGame()"
          v-if="!showOptions"
        >
          START A GAME {{ gameMode }}
        </div>
        <div v-else class="flex-row">
          <div
            class="btn"
            :class="{ loading: loadingStartGame }"
            v-on:click="startGame('easy')"
          >
            EASY
          </div>
          <div
            class="btn"
            :class="{ loading: loadingStartGame }"
            v-on:click="startGame('medium')"
          >
            MEDIUM
          </div>
          <div
            class="btn"
            :class="{ loading: loadingStartGame }"
            v-on:click="startGame('hard')"
          >
            HARD
          </div>
        </div>
      </transition>
    </div>
  </div>
</template>

<script>
import user from "@/components/user.vue";
import addUser from "@/components/addUser.vue";
import { send } from "@/ws_client.js";
export default {
  components: {
    user,
    addUser
  },
  data() {
    return {
      loadingStartGame: false,
      showOptions: false
    };
  },
  methods: {
    prepareGame() {
      this.loadingStartGame = true;
      send(
        {
          type: "ReadyGame"
        },
        () => {
          this.loadingStartGame = false;
          this.showOptions = true;
        }
      );
    },
    startGame(difficulty) {
      this.loadingStartGame = true;
      send(
        {
          type: "StartGame",
          difficulty: difficulty
        },
        response => {
          if (response.status == 200) {
            this.$router.push("/countdown");
          }
          this.loadingStartGame = false;
          this.showOptions = true;
        }
      );
    }
  },
  computed: {
    users() {
      return this.$store.getters.getUsers;
    },
    pistols() {
      return this.$store.getters.getPistols;
    },
    gameMode() {
      if (this.users.length == 1) {
        return "( SOLO )";
      } else if (this.users.length > 1) {
        return "( " + this.users.length + " USERS )";
      } else {
        return "";
      }
    }
  }
};
</script>

<style lang="scss" scoped>
.page{
  width: 100%;
  height: 100%;
  flex-grow: 2;
  display: flex;
  flex-direction: column;
}
.home-content {
  display: flex;
  flex-direction: column;
  .grow {
    flex-grow: 2;
  }
  & > .btn {
    flex-grow: 0;
    flex-shrink: 0;
  }
}
.flex-row {
  display: flex;
  gap: var(--element-spacing);
  overflow-x: auto;
  margin: calc(var(--element-spacing) * -1) calc(var(--card-spacing) * -1) 0
    calc(var(--card-spacing) * -1);
  padding: var(--element-spacing) var(--card-spacing);
  & > * {
    margin-bottom: 0;
  }
  &::-webkit-scrollbar {
    display: none;
  }
  &::after {
    content: ".";
    display: block;
    visibility: hidden;
    width: 0;
    margin-left: calc(var(--card-spacing) - var(--element-spacing));
  }
}
.pistol {
  width: 225px;
  text-overflow: ellipsis;
  white-space: nowrap;
  position: relative;
  & > * {
    position: relative;
    z-index: 2;
  }
  &::before {
    transition: top 300ms ease, background-color 300ms ease,
      box-shadow 300ms ease, border-color 300ms ease;
    z-index: 0;
    content: "";
    position: absolute;
    display: block;
    right: -2px;
    left: -2px;
    bottom: -2px;
    top: calc((100 - var(--i)) * 1% - 2px);
    background-color: var(--primary-2);

    border: 2px solid var(--primary-2);
    box-shadow: 0 0 16px 0px var(--primary-5),
      inset 0 0 16px 0px var(--primary-3);
  }
  &.error {
    &::before {
      background-color: var(--error-2);

      border: 2px solid var(--error-2);
      box-shadow: 0 0 16px 0px var(--error-5), inset 0 0 16px 0px var(--error-3);
    }
  }
  &.good {
    &::before {
      background-color: var(--good-2);

      border: 2px solid var(--good-2);
      box-shadow: 0 0 16px 0px var(--good-5), inset 0 0 16px 0px var(--good-3);
    }
  }
}
</style>
