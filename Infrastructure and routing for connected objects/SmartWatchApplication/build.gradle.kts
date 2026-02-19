buildscript {
    dependencies {
        classpath(libs.dagger.hilt.android.gradle.plugin)
        classpath(libs.androidx.navigation.safe.args.gradle.plugin)
      //  classpath(libs.google.services)
        //classpath("com.google.firebase:firebase-crashlytics-gradle:3.0.6")
    }
}

plugins {
    alias(libs.plugins.android.application) apply false
    alias(libs.plugins.kotlin.android) apply false
    id("com.google.devtools.ksp") version "2.1.10-1.0.29" apply false
    id("com.google.dagger.hilt.android") version "2.57.2" apply false
    id("com.google.android.libraries.mapsplatform.secrets-gradle-plugin") version ("2.0.1") apply false
    id("org.jetbrains.kotlin.plugin.serialization") version ("1.6.21")


}