<template>
  <div class="addUser btn ritch" v-on:click="openModal()">
    <svg class="icon" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 448 512">
      <path
        fill="currentColor"
        d="M416 208H272V64c0-17.67-14.33-32-32-32h-32c-17.67 0-32 14.33-32 32v144H32c-17.67 0-32 14.33-32 32v32c0 17.67 14.33 32 32 32h144v144c0 17.67 14.33 32 32 32h32c17.67 0 32-14.33 32-32V304h144c17.67 0 32-14.33 32-32v-32c0-17.67-14.33-32-32-32z"
      ></path>
    </svg>
    <span>ADD USER</span>
  </div>

  <Teleport to="#app">
    <transition name="fade">
      <div class="modal" v-if="showAddUserModal">
        <div class="modal-body box">
          <h1>Add a user</h1>
          <div class="box">
            <h2>Add existing user</h2>
            <p v-if="signInUser.error.code != ''">
              <span class="pill error">{{ signInUser.error.code }}</span>
              <span>{{ signInUser.error.msg }}</span>
            </p>
            <h5>Email</h5>
            <input
              class="sm"
              type="text"
              v-model="signInUser.email"
              :class="{ disabled: addingUser }"
              v-on:keyup.enter="signInUserFunction()"
            />
            <h5>Password</h5>
            <input
              class="sm"
              type="password"
              v-model="signInUser.password"
              :class="{ disabled: addingUser }"
              v-on:keyup.enter="signInUserFunction()"
            />
            <div class="actions">
              <div
                class="btn sm"
                :class="{
                  disabled:
                    creatingUser ||
                    signInUser.email == '' ||
                    signInUser.password == '',
                  loading: addingUser
                }"
                v-on:click="signInUserFunction()"
              >
                SIGN IN
              </div>
            </div>
          </div>

          <div class="box">
            <h2>Create a new user</h2>
            <p v-if="createUser.error.code != ''">
              <span class="pill error">{{ createUser.error.code }}</span>
              <span>{{ createUser.error.msg }}</span>
            </p>
            <h5>Name</h5>
            <input
              class="sm"
              type="text"
              v-model="createUser.name"
              :class="{ disabled: creatingUser }"
              v-on:keyup.enter="createUserFunction()"
            />
            <h5>Email</h5>
            <input
              class="sm"
              type="text"
              v-model="createUser.email"
              :class="{ disabled: creatingUser }"
              v-on:keyup.enter="createUserFunction()"
            />
            <h5>Password</h5>
            <input
              class="sm"
              type="password"
              v-model="createUser.password"
              :class="{ disabled: creatingUser }"
              v-on:keyup.enter="createUserFunction()"
            />
            <div class="actions">
              <div
                class="btn sm"
                :class="{
                  disabled:
                    addingUser ||
                    createUser.name == '' ||
                    createUser.email == '' ||
                    createUser.password == '',
                  loading: creatingUser
                }"
                v-on:click="createUserFunction()"
              >
                SIGN IN
              </div>
            </div>
          </div>

          <div class="actions">
            <div
              class="btn sm"
              v-on:click="showAddUserModal = false"
              :class="{ disabled: creatingUser || addingUser }"
            >
              CLOSE
            </div>
          </div>
        </div>
      </div>
    </transition>
  </Teleport>
</template>

<script>
import { send } from "@/ws_client.js";
export default {
  data() {
    return {
      showAddUserModal: false,
      creatingUser: false,
      addingUser: false,
      createUser: {
        name: "",
        email: "",
        password: "",
        error: {
          code: "",
          msg: ""
        }
      },
      signInUser: {
        email: "",
        password: "",
        error: {
          code: "",
          msg: ""
        }
      }
    };
  },
  methods: {
    signInUserFunction() {
      if(this.signInUser.email == '' || this.signInUser.password == ''){
        return;
      }
      this.addingUser = true;
      this.signInUser.error = {
        code: "",
        msg: ""
      };
      send(
        {
          type: "RequestSignin",
          email: this.signInUser.email,
          password: this.signInUser.password
        },
        response => {
          if (response.status != 200) {
            console.log("Sign in response: ", response);
            this.signInUser.error.code = response.status;
            switch (response.status) {
              case 401:
                this.signInUser.error.msg = "Password incorrect.";
                break;
              case 404:
                this.signInUser.error.msg = "Email was not found.";
                break;
              case 208:
                this.signInUser.error.msg = "User is already signed in.";
                break;
            }
          } else {
            this.signInUser.error = {
              code: "",
              msg: ""
            };
            this.showAddUserModal = false;
          }
          this.addingUser = false;
        }
      );
    },
    createUserFunction() {
      if(this.createUser.email == '' || this.createUser.password == '' || this.createUser.name == ''){
        return;
      }
      this.creatingUser = true;
      this.createUser.error = {
        code: "",
        msg: ""
      };
      send(
        {
          type: "RequestCreateUser",
          email: this.createUser.email,
          password: this.createUser.password,
          name: this.createUser.name
        },
        response => {
          if (response.status != 200) {
            console.log("Sign in response: ", response);
            this.createUser.error.code = response.status;
            switch (response.status) {
              case 400:
                this.createUser.error.msg = "Email is already in use.";
                break;
            }
          } else {
            this.createUser.error = {
              code: "",
              msg: ""
            };
            this.showAddUserModal = false;
          }
          this.creatingUser = false;
        }
      );
    },
    openModal(){
      this.createUser = {
        name: "",
        email: "",
        password: "",
        error: {
          code: "",
          msg: ""
        }
      };
      this.signInUser = {
        email: "",
        password: "",
        error: {
          code: "",
          msg: ""
        }
      };

      this.showAddUserModal = true;
    }
  }
};
</script>

<style lang="scss" scoped>
.addUser {
  width: 225px;
  height: 225px;
  overflow: hidden;
  flex-shrink: 0;
  flex-grow: 0;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  gap: 1rem;
  .icon {
    height: 4rem;
    flex-grow: 0;
    opacity: 0.8;
  }
}
</style>
