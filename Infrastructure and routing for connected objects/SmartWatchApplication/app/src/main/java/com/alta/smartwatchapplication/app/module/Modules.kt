package com.alta.altapfproject.app.module

import android.content.Context

import com.alta.smartwatchapplication.repository.DataRepository
import com.alta.smartwatchapplication.repository.InternetConnectRepository

import dagger.Module
import dagger.Provides
import dagger.hilt.InstallIn
import dagger.hilt.android.qualifiers.ApplicationContext
import dagger.hilt.components.SingletonComponent
import javax.inject.Singleton


@Module
@InstallIn(SingletonComponent::class)
object Modules {




    @Provides
    @Singleton
    fun provideDataRepository(
        @ApplicationContext context: Context,

    ): DataRepository {
        return DataRepository(
            context,

        )
    }

    @Provides
    @Singleton
    fun provideInternetConnectRepository(
        @ApplicationContext context: Context,
    ): InternetConnectRepository {
        return InternetConnectRepository(context)
    }

    /*  @Provides
      @Singleton
      fun provideDatabase(@ApplicationContext context: Context): DatabaseCamera {
          return Room.databaseBuilder(
              context,
              DatabaseCamera::class.java,
              DATABASE_NAME
          )
              .fallbackToDestructiveMigration()
              .build()
      }

      @Singleton
      @Provides
      fun provideNoteTableDao(databaseCamera: DatabaseCamera): CustomLocationDao {
          return databaseCamera.customLocationDao()
      }*/

}