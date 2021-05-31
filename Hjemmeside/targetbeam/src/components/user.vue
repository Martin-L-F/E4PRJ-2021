<template>
  <div class="user">
    <Gravatar :size="225" :email="email" />
    <div class="edit" v-on:click="openModal()">
      <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 512 512">
        <path
          fill="currentColor"
          d="M290.74 93.24l128.02 128.02-277.99 277.99-114.14 12.6C11.35 513.54-1.56 500.62.14 485.34l12.7-114.22 277.9-277.88zm207.2-19.06l-60.11-60.11c-18.75-18.75-49.16-18.75-67.91 0l-56.55 56.55 128.02 128.02 56.55-56.55c18.75-18.76 18.75-49.16 0-67.91z"
        ></path>
      </svg>
    </div>
    <div class="top">
      <div class="line"></div>
      <div class="line"></div>
      <div class="line"></div>
      <div class="line"></div>
      <div class="line"></div>
      <div class="line"></div>
    </div>
    <p class="bottom">{{ name }}</p>
  </div>
  <Teleport to="#app">
    <transition name="fade">
      <div class="modal" v-if="showModal">
        <div class="modal-body box">
          <h1>Edit user information</h1>
          <div class="box">
            <h2>Name</h2>
            <p v-if="changeNameError.code != ''">
              <span
                class="pill good"
                :class="{ error: changeNameError.code != 200 }"
                >{{ changeNameError.code }}</span
              >
              <span>{{ changeNameError.msg }}</span>
            </p>
            <div class="content-flex">
              <input
                class="sm"
                type="text"
                v-model="tempName"
                :class="{ disabled: changingName }"
                v-on:keyup.enter="changeName()"
              />
              <div
                class="btn sm"
                :class="{
                  disabled: tempName == name || tempName == '',
                  loading: changingName
                }"
                v-on:click="changeName()"
              >
                SAVE
              </div>
            </div>
          </div>
          <div class="box">
            <h2>Change password</h2>
            <p v-if="changePassError.code != ''">
              <span
                class="pill good"
                :class="{ error: changePassError.code != 200 }"
                >{{ changePassError.code }}</span
              >
              <span>{{ changePassError.msg }}</span>
            </p>
            <h5>Old password</h5>
            <input
              class="sm"
              type="password"
              v-model="oldPass"
              :class="{ disabled: changingPass }"
              v-on:keyup.enter="changePass()"
            />
            <h5>New password</h5>
            <input
              class="sm"
              type="password"
              v-model="newPass"
              :class="{ disabled: changingPass }"
              v-on:keyup.enter="changePass()"
            />
            <div class="actions">
              <div
                class="btn sm"
                :class="{
                  loading: changingPass,
                  disabled: oldPass == newPass || newPass == '' || oldPass == ''
                }"
                v-on:click="changePass()"
              >
                CHANGE
              </div>
            </div>
          </div>

          <div class="actions">
            <div
              class="btn sm"
              :class="{ loading: signingOut }"
              v-on:click="signout()"
            >
              SIGN OUT
            </div>
            <div class="btn sm" v-on:click="showModal = false" :class="{}">
              CLOSE
            </div>
          </div>
        </div>
      </div>
    </transition>
  </Teleport>
</template>

<script>
import Gravatar from "@/components/gravatar.vue";
import { send } from "@/ws_client.js";
export default {
  components: {
    Gravatar
  },
  data() {
    return {
      showModal: false,
      signingOut: false,

      tempName: "",
      changingName: false,
      changeNameError: {
        code: "",
        msg: ""
      },

      changingPass: false,
      oldPass: "",
      newPass: "",
      changePassError: {
        code: "",
        msg: ""
      }
    };
  },
  methods: {
    openModal: function(){
      this.tempName = this.name;
      this.changeNameError = {
        code: "",
        msg: ""
      };
      this.oldPass = "";
      this.newPass = "";
      this.changePassError = {
        code: "",
        msg: ""
      };
      this.showModal = true;
    },
    signout: function() {
      this.signingOut = true;
      send(
        {
          type: "RequestSignOut",
          email: this.email
        },
        () => {
          this.signingOut = false;
        }
      );
    },
    changeName: function() {
      if(this.tempName == ''){
        return;
      }
      this.changingName = true;
      send(
        {
          type: "RequestChangeName",
          email: this.email,
          name: this.tempName
        },
        response => {
          if(response.status == 200){
            this.changeNameError.code = 200;
            this.changeNameError.msg = "Sucess.";
          }else{
            this.changeNameError.code = response.status;
            this.changeNameError.msg = "Failed.";
          }
          this.tempName = this.name;
          this.changingName = false;
        }
      );
    },
    changePass() {
      if(this.oldPass == '' || this.newPass == ''){
        return;
      }
      this.changingPass = true;
      send(
        {
          type: "RequestChangePassword",
          email: this.email,
          oldPassword: this.oldPass,
          newPassword: this.newPass
        },
        response => {
          console.log("Change password response: ", response);
          if (response.status == 401) {
            this.changePassError.code = 401;
            this.changePassError.msg = "Password Incorrect.";
          } else if (response.status == 200) {
            this.changePassError.code = 200;
            this.changePassError.msg = "Sucess.";
          } else {
            this.changePassError.code = "";
            this.changePassError.msg = "";
          }
          this.oldPass = "";
          this.newPass = "";
          this.changingPass = false;
        }
      );
    }
  },
  props: {
    email: {
      default: "",
      type: String
    },
    name: {
      default: "",
      type: String
    }
  },
  mounted() {
    this.tempName = this.name;
  }
};
</script>

<style lang="scss" scoped>
.user {
  width: 225px;
  height: 225px;
  position: relative;
  overflow: hidden;
  box-shadow: 0 0 10px 0px var(--white-3);
  flex-shrink: 0;

  .gravatar {
    position: absolute;
    &,
    & * {
      user-select: none;
      pointer-events: none;
    }
  }

  div {
    position: relative;
    z-index: 1;
  }

  &::after {
    content: "";
    z-index: 2;
    position: absolute;
    border: 2px solid var(--white-3);
    box-shadow: inset 0 0 10px 0px var(--white-1);
    top: 0;
    right: 0;
    bottom: 0;
    left: 0;
    user-select: none;
    pointer-events: none;
  }
  display: flex;
  flex-direction: column;
  align-items: stretch;
  justify-content: stretch;
  --spacing: 12px;

  .edit {
    position: absolute;
    top: 0;
    right: 0;
    padding: 12px;
    width: 42px;
    height: 42px;
    * {
      user-select: none;
      pointer-events: none;
    }
    cursor: pointer;
    transition: border-color 300ms ease, box-shadow 300ms ease, color 300ms ease,
      background-color 300ms ease;
    &:hover:not(.disabled) {
      color: var(--primary);
      border-color: var(--primary);
      box-shadow: 0 0 16px 0px var(--primary-5),
        inset 0 0 16px 0px var(--primary-5);
      background-color: var(--primary-1);
    }

    &:active:not(.disabled) {
      box-shadow: 0 0 10px 0px var(--primary-3),
        inset 0 0 10px 0px var(--primary-3);
      border-color: var(--primary-7);
    }
  }
  .top {
    &,
    & * {
      user-select: none;
      pointer-events: none;
    }
    flex-grow: 2;
    z-index: 4;
    position: relative;
    .line {
      z-index: 2;
      position: absolute;
      background: var(--white-3);
      box-shadow: 0 0 10px 0px var(--white-3);
      --btn-size: 30px;

      &:nth-of-type(1) {
        top: calc(var(--spacing) + 2px);
        left: var(--spacing);
        bottom: 0;
        width: 2px;
      }
      &:nth-of-type(2) {
        top: var(--spacing);
        left: var(--spacing);
        right: calc(var(--spacing) + var(--btn-size));
        height: 2px;
      }
      &:nth-of-type(3) {
        bottom: 0;
        left: var(--spacing);
        right: var(--spacing);
        height: 2px;
      }
      &:nth-of-type(4) {
        bottom: 2px;
        top: calc(var(--spacing) + var(--btn-size));
        right: var(--spacing);
        width: 2px;
      }
      &:nth-of-type(5) {
        top: calc(var(--spacing) + 2px);
        right: calc(var(--spacing) + var(--btn-size));
        width: 2px;
        height: var(--btn-size);
      }
      &:nth-of-type(6) {
        top: calc(var(--spacing) + var(--btn-size));
        right: calc(var(--spacing) + 2px);
        width: calc(var(--btn-size) - 2px);
        height: 2px;
      }
    }
  }
  .bottom {
    user-select: none;
    pointer-events: none;
    padding: var(--spacing);
    overflow: hidden;
    white-space: nowrap;
    text-overflow: ellipsis;
    background-image: linear-gradient(to bottom, transparent, rgba(black, 0.7));
    position: relative;
    z-index: 1;
    &::after {
      content: "";
    }
  }
}

.content-flex {
  display: flex;
  gap: var(--element-spacing);
  * {
    margin-bottom: 0;
  }
  &:not(:last-child) {
    margin-bottom: var(--element-spacing);
  }
}
</style>
